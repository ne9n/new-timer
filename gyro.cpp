#include "gyro.h"
#include "cheesehead_timer.h"
#include <Servo.h>
#include "state_machine.h"
#include <limits.h>
#include <EEPROM.h>
//#include <Arduino.h>
#define rLIMIT 150
extern unsigned long currentMillis;
MPU6050 mpu6050(Wire);

// Mocking support for tests: when `useMockMPU` is true the getAngle*_read
// helpers return mocked values instead of reading the real MPU.
static bool useMockMPU = false;
static volatile int mockX = 0;
static volatile int mockY = 0;
static volatile int mockZ = 0;

void setMockMPU(bool v) { useMockMPU = v; }
void setMockAngles(int x, int y, int z) { mockX = x; mockY = y; mockZ = z; }

int getAngleX_read() { return useMockMPU ? mockX : mpu6050.getAngleX(); }
int getAngleY_read() { return useMockMPU ? mockY : mpu6050.getAngleY(); }
int getAngleZ_read() { return useMockMPU ? mockZ : mpu6050.getAngleZ(); }

// Simple serial command processor to accept test commands over Serial:
//  - "TESTMODE ON" / "TESTMODE OFF"
//  - "MPU X Y Z" to set angles (integers)
void processSerialMPU()
{
  while (Serial.available())
  {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    if (line.equalsIgnoreCase("TESTMODE ON")) { setMockMPU(true); Serial.println("ACK TEST ON"); continue; }
    if (line.equalsIgnoreCase("TESTMODE OFF")) { setMockMPU(false); Serial.println("ACK TEST OFF"); continue; }

    if (line.startsWith("MPU"))
    {
      int x=0,y=0,z=0;
      int n = sscanf(line.c_str(), "MPU %d %d %d", &x, &y, &z);
      if (n == 3) { setMockAngles(x,y,z); Serial.println("ACK MPU"); }
      else { Serial.println("ERR PARSE"); }
    }
  }
}

// servo declared in main sketch
extern Servo esc;

#if defined(__AVR__)
#include <avr/pgmspace.h>
#endif

#if defined(__AVR__)
#define READ_SIN(i) pgm_read_word_near(sinetbl + (i))
#else
#define READ_SIN(i) (sinetbl[(i)])
#endif

int iangleX;
int iangleY;
int iangleZ;
int posTrim = 0;
int maneuverBoost = 0;
// last sample storage - initialize safely
unsigned long last_mils = 0;
int last_x = 0;
int last_y = 0;
int last_z = 0;
bool PitchEX = false;
bool YawEX = false;
bool YawLOW = false;

// Hysteresis counters for pitch excessive detection
static int pitchSetCounter = 0;
static int pitchClearCounter = 0;
static const int PITCH_SET_N = 2;   // require 2 consecutive windows to set
static const int PITCH_CLEAR_M = 5; // require 5 consecutive safe windows to clear

// Slow yaw counting after takeoff
static int yawSlowCount = 0;
static const int YAW_SLOW_THRESHOLD = 50; // samples -> tune (50 * GYRO_SAMPLE_MS)

extern speed_state ispeed_state;
extern bool run_state;
// yaw hysteresis counters for high/low conditions
static int yawHighCounter = 0;
static int yawLowCounter = 0;
static const int YAW_HIGH_N = 2; // require 2 consecutive high-delta windows to trigger
static const int YAW_LOW_M = 5;  // require 5 consecutive low-abs windows to trigger

// Tunable parameters
static const unsigned long GYRO_SAMPLE_MS = 100;      // sample interval in ms
static const int YAW_LOW_THRESHOLD = 10;             // low yaw absolute

// Use thresholds from EEPROM-configured `TimerSetup` when available.
extern param TimerSetup;
// Fallback defaults if TimerSetup not yet initialized
static const int DEFAULT_PITCH_DELTA_THRESHOLD = 40;         // degrees change -> excessive
static const int DEFAULT_YAW_DELTA_RATE_THRESHOLD = 20; // deg/sec

void mpu_setup()
{

 
 mpu6050.begin();
 mpu6050.calcGyroOffsets(true);
// mpu6050.calcGyroOffsets(true);
//  mpu6050.setGyroOffsets(TimerSetup.calX / 100.0, TimerSetup.calY / 100.0, TimerSetup.calZ / 100.0);
}
void setUpMPU(void)
{
  // Perform an automatic calibration by sampling angle deltas while
  // the board is stationary. Compute average gyro bias (deg/sec),
  // apply to the library, and save into TimerSetup (scaled x100).
  const int SAMPLES = 200;
  const unsigned long SAMPLE_MS = 10; // sample interval

  Serial.println(F("Starting MPU auto-calibration: keep board still."));
  // warm up
  for (int i = 0; i < 5; ++i) { mpu6050.update(); delay(20); }

  int prevX = mpu6050.getAngleX();
  int prevY = mpu6050.getAngleY();
  int prevZ = mpu6050.getAngleZ();
  double accRateX = 0.0;
  double accRateY = 0.0;
  double accRateZ = 0.0;

  for (int i = 0; i < SAMPLES; ++i)
  {
    mpu6050.update();
    int ax = mpu6050.getAngleX();
    int ay = mpu6050.getAngleY();
    int az = mpu6050.getAngleZ();

    auto signedDelta = [](int cur, int prev)->int {
      int d = cur - prev;
      if (d > 180) d -= 360;
      else if (d < -180) d += 360;
      return d;
    };

    int dx = signedDelta(ax, prevX);
    int dy = signedDelta(ay, prevY);
    int dz = signedDelta(az, prevZ);

    double rateX = (double)dx * 1000.0 / (double)SAMPLE_MS; // deg/sec
    double rateY = (double)dy * 1000.0 / (double)SAMPLE_MS;
    double rateZ = (double)dz * 1000.0 / (double)SAMPLE_MS;

    accRateX += rateX;
    accRateY += rateY;
    accRateZ += rateZ;

    prevX = ax; prevY = ay; prevZ = az;
    delay(SAMPLE_MS);
  }

  double biasX = accRateX / (double)SAMPLES;
  double biasY = accRateY / (double)SAMPLES;
  double biasZ = accRateZ / (double)SAMPLES;

  // Apply offsets to the MPU library (if available) and persist in EEPROM
  // Library expects offsets in deg/sec; apply them directly.
  mpu6050.setGyroOffsets((float)biasX, (float)biasY, (float)biasZ);

  // Store scaled integer with factor 100 to match existing code convention
  TimerSetup.calX = (int)(biasX * 100.0);
  TimerSetup.calY = (int)(biasY * 100.0);
  TimerSetup.calZ = (int)(biasZ * 100.0);

  int eeAddress = 0;
  EEPROM.put(eeAddress, TimerSetup);
  // Commit for platforms that require it (ESP32)
#if defined(ESP32)
  EEPROM.commit();
#endif

  Serial.print(F("Calibration complete. Offsets (deg/s): "));
  Serial.print(biasX); Serial.print(F(","));
  Serial.print(biasY); Serial.print(F(","));
  Serial.println(biasZ);
  Serial.print(F("Saved TimerSetup.calX/calY/calZ = "));
  Serial.print(TimerSetup.calX); Serial.print(F(","));
  Serial.print(TimerSetup.calY); Serial.print(F(","));
  Serial.println(TimerSetup.calZ);
}

 // mpu6050.calcGyroOffsets(true);
void read_giro()
{
  // read raw angles
  iangleX = getAngleX_read();
  iangleY = getAngleY_read();
  iangleZ = getAngleZ_read();

  // initialize on first call
  if (last_mils == 0)
  {
    last_mils = currentMillis;
    last_x = iangleX;
    last_y = iangleY;
    last_z = iangleZ;
    PitchEX = false;
    YawEX = false;
    YawLOW = false;
    return;
  }

  // only evaluate every GYRO_SAMPLE_MS
  if ((currentMillis - last_mils) < GYRO_SAMPLE_MS) return;

  int dx = iangleX - last_x;
  int dy = iangleY - last_y;
  int dz = iangleZ - last_z;

  // --- LAP COUNT (tenths) ---
  static float accumulatedY = 0.0f; // accumulated signed degrees
  auto signedDelta = [](int cur, int prev)->int {
    int d = cur - prev;
    if (d > 180) d -= 360;
    else if (d < -180) d += 360;
    return d;
  };

  int signedDeltaY = signedDelta(iangleY, last_y);
  accumulatedY += (float)signedDeltaY;

  auto processLapAccum = [&]() {
    // 36 degrees == 0.1 lap
    if (fabs(accumulatedY) < 36.0f) return;
    int tenths = (int)(fabs(accumulatedY) / 36.0f);
    TimerSetup.LapCount += (unsigned long)tenths; // units = 0.1 lap

    // enforce shutdown if limit reached
    if (TimerSetup.LapLimit > 0 && TimerSetup.LapCount >= TimerSetup.LapLimit)
    {
      Serial.println(F("Lap limit exceeded - shutting down"));
      run_state = false;
      esc.write(0);
      digitalWrite(LED3, LOW);
      digitalWrite(LED4, LOW);
      digitalWrite(LED5, LOW);
    }

    // trigger BURP one full lap (10 tenths) before limit while flying
    if (TimerSetup.LapLimit >= 10 && ispeed_state == speed_state::FLY)
    {
      unsigned long threshold = TimerSetup.LapLimit - 10;
      if (TimerSetup.LapCount >= threshold && ispeed_state != speed_state::BURP && run_state)
      {
        Serial.println(F("Approaching lap limit - switching to BURP"));
        setSpeedState(speed_state::BURP);
      }
    }

    float consumed = 36.0f * (float)tenths;
    if (accumulatedY > 0) accumulatedY -= consumed; else accumulatedY += consumed;
  };

  processLapAccum();

  // --- PITCH / YAW EVALUATION ---
  evaluateHighPitchYaw(dx, dy, dz);

  // update last samples and time
  last_x = iangleX;
  last_y = iangleY;
  last_z = iangleZ;
  last_mils = currentMillis;
}

// Refactored function to evaluate high pitch and yaw events
void evaluateHighPitchYaw(int dx, int dy, int dz)
{
  (void)dy; // dy intentionally unused in current algorithm; silence compiler warning
  // Simplified, clearer handling split into pitch and yaw sections.

  // --- PITCH (hysteresis) ---
  int pitchThresh = (TimerSetup.PitchExThresh > 0) ? TimerSetup.PitchExThresh : DEFAULT_PITCH_DELTA_THRESHOLD;
  if (abs(dx) >= pitchThresh)
  {
    pitchSetCounter++;
    pitchClearCounter = 0;
  }
  else
  {
    pitchSetCounter = 0;
    if (abs(dx) < (pitchThresh / 2)) pitchClearCounter++; else pitchClearCounter = 0;
  }

  if (pitchSetCounter >= PITCH_SET_N)
  {
    PitchEX = true;
    run_state = false;
  }
  else if (pitchClearCounter >= PITCH_CLEAR_M)
  {
    PitchEX = false;
  }

  // --- YAW (rate and absolute) ---
  if (ispeed_state == speed_state::TAKEOFF || ispeed_state == speed_state::FLY)
  {
    float yawRate = (float)dz * 1000.0f / (float)GYRO_SAMPLE_MS;
    int yawRateThresh = (TimerSetup.YawRateExThresh > 0) ? TimerSetup.YawRateExThresh : DEFAULT_YAW_DELTA_RATE_THRESHOLD;

    if (abs(yawRate) >= yawRateThresh)
    {
      yawHighCounter++;
      yawLowCounter = 0;
    }
    else if (abs(iangleZ) <= YAW_LOW_THRESHOLD)
    {
      yawLowCounter++;
      yawHighCounter = 0;
    }
    else
    {
      yawHighCounter = 0;
      yawLowCounter = 0;
    }

    if (ispeed_state == speed_state::TAKEOFF)
    {
      if (yawSlowCount < INT_MAX - 1) yawSlowCount++;
    }

    bool disableFlightChecks = (ispeed_state == speed_state::FLY && digitalRead(DS3) == LOW);

    if (yawHighCounter >= YAW_HIGH_N)
    {
      if (!disableFlightChecks) { YawEX = true; run_state = false; }
    }
    else if (yawLowCounter >= YAW_LOW_M)
    {
      if (!disableFlightChecks) { YawLOW = true; run_state = false; }
    }
    else if (yawSlowCount >= YAW_SLOW_THRESHOLD)
    {
      if (!disableFlightChecks) { YawEX = true; run_state = false; }
    }
    else
    {
      YawEX = false;
      YawLOW = false;
    }
  }
  else
  {
    if (yawSlowCount > 0) yawSlowCount--;
    yawHighCounter = 0;
    yawLowCounter = 0;
    YawEX = false;
    YawLOW = false;
  }
}

void speedGyro()
{
  mpu6050.update();

  // update raw angles
  iangleX = getAngleX_read();
  iangleY = getAngleY_read();
  iangleZ = getAngleZ_read();
  // compute pitch trim from pitch angle (`iangleX`) using sin-table for smooth curve
  // - Uses `TimerSetup.px` as max magnitude (0..180)
  // - Signed sin table: READ_SIN gives ~0..1000, subtract 500 -> -500..+500
  // - Map -500..+500 proportionally to -px..+px, then invert sign so nose-up reduces throttle
  auto angleToIndex = [](int deg)->int {
    int d = deg % 360;
    if (d < 0) d += 360;
    int idx = (d / 10) % SIN_TBL_SZ; // 10-degree steps
    return idx;
  };

  int idx = angleToIndex(iangleX);
  int sinVal = READ_SIN(idx); // 0..~1000
  int signedSin = sinVal - 500; // -500..+500
  float trimf = ((float)signedSin / 500.0f) * (float)TimerSetup.px; // -px..+px
  // invert sign per preference: nose-up (positive pitch) should lower throttle
  int trimmed = -(int)trimf;
  // clamp to +/- px
  int maxTrim = (int)TimerSetup.px;
  if (trimmed > maxTrim) trimmed = maxTrim;
  if (trimmed < -maxTrim) trimmed = -maxTrim;
  posTrim = trimmed;

  // compute maneuver boost from pitch angle magnitude using TimerSetup.rx (0..180)
  // small deadband to avoid jitter
  int pitchMag = abs(iangleX);
  if (pitchMag < 5 || TimerSetup.rx == 0)
  {
    maneuverBoost = 0;
  }
  else
  {
    float boostf = ((float)pitchMag / 90.0f) * (float)TimerSetup.rx; // scale to rx
    if (boostf < 0) boostf = 0;
    if (boostf > (float)TimerSetup.rx) boostf = (float)TimerSetup.rx;
    maneuverBoost = (int)(boostf + 0.5f);
  }
}


















