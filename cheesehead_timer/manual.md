# Cheesehead Timer OSH26 — User Manual & Setup Guide

## Overview

This project (`cheesehead_timer`) is a control-line throttle sequencer for model aircraft. It manages arming, takeoff ramp, flight timing, BURP (short throttle burst), lap counting (tenths-of-a-lap), and safety shutdowns based on pitch/yaw detection.

Files of interest:
- `cheesehead_timer.ino` — main sketch and setup/loop
- `gyro.cpp` / `gyro.h` — MPU6050 reads, lap counting, `posTrim`, `maneuverBoost`, safety checks
- `state_machine.cpp` / `state_machine.h` — state transitions and throttle output
- `term.cpp` / `term.h` — serial menu for configuration and telemetry
- `doc.txt`, `manual.md`, `tests.txt`, `bench_integration_test_guide.md` — documentation and test guides

## Hardware & Wiring (summary)

- ESC / Servo PWM: `SERVO` (pin defined in `cheesehead_timer.h`, typically D3)
- Dip switches: `DS1`, `DS2`, `DS3` (DS3 disables some flight checks when LOW)
- LEDs: `LED3`, `LED4`, `LED5` (status indicators)
- Button: `BUTTONPIN`
- MPU6050: connect SDA / SCL to appropriate MCU pins and `Wire` I2C

Refer to your board's pinout and `cheesehead_timer.h` for the actual pin defines.

## Software Setup (Windows, CLI)

1. Install Git (if you plan to push code) and `arduino-cli` (if not already installed).

2. From an elevated PowerShell (if needed) install Arduino CLI via winget:

```powershell
winget install ArduinoSA.CLI -e --source winget
```

3. Add AVR core (if not already):

```powershell
arduino-cli core update-index
arduino-cli core install arduino:avr
```

4. Compile for Arduino Nano (example FQBN):

```powershell
cd "C:\Users\310323\Desktop\cheeshead_timer"
arduino-cli compile --fqbn arduino:avr:nano .
```

5. Upload to a connected board (replace COM port):

```powershell
arduino-cli upload --fqbn arduino:avr:nano -p COM3 -t .
```

If you use the Arduino IDE, open the folder and select the correct board and port, then Compile / Upload.

## Python Desktop Application Setup

The project includes a Python/CustomTkinter desktop GUI for live telemetry plotting, state gauge monitoring, and automated firmware flashing.

### 1. Requirements
* **Python**: Python 3.8+ (tested on Python 3.10–3.13)
* **Dependencies**:
  * `pyserial` — serial port communication
  * `customtkinter` — modern dark-themed GUI interface

### 2. Installing Dependencies
Install required packages using `requirements.txt`:
```powershell
pip install -r requirements.txt
```
or install directly via `pip`:
```powershell
pip install pyserial customtkinter
```

### 3. Launching the App
From the project root directory:
```powershell
python cheesehead_timer/desktop_app.py
```

## Serial Terminal & Menu (19200 baud)

- Open serial at 19200 baud.
- Press `?` to display the menu.

Common commands:
- `?` — show menu
- `l` — reset `LapCount` to zero
- `M <value>` — set `LapLimit` as decimal laps (fractional allowed). Example: `M 1.5` stores 15 (tenths)
- `E <value>` — set pitch gain `px` (affects `posTrim` magnitude)
- `K <value>` — set maneuver gain `rx` (affects `maneuverBoost`)
- `s` — save current `TimerSetup` to EEPROM
- `G` — run gyro calibration routine
- `o`/`O` — toggle LED outputs

Telemetry fields shown in the menu include:
- `LapCount` / `LapLimit` (displayed as decimal laps)
- `P  Pos trim` — current pitch trim applied to throttle
- `B  Maneuver boost` — current maneuver boost applied
- `1, 2, 3` — axis mapping configuration (Pitch, Roll, Yaw)

## Axis Mapping (X, Y, Z)

You can customize which physical MPU6050 axis (X=0, Y=1, Z=2) corresponds to Pitch, Roll, and Yaw in the software.

Commands:
- `1 <value>` — Set **Pitch** axis (0, 1, or 2). Default is 0 (X).
- `2 <value>` — Set **Roll** axis (0, 1, or 2). Default is 1 (Y).
- `3 <value>` — Set **Yaw** axis (0, 1, or 2). Default is 2 (Z).

Example: To set Yaw to the X-axis, type `3 0`.
Remember to send `s` to save your mapping to EEPROM.

## Lap Counting & Lap Limit

- Lap counting uses MPU6050 yaw (`iangleY`) deltas and accumulates signed rotation; every 36° counts as 0.1 lap (tenths).
- `TimerSetup.LapCount` is stored in tenths (1 = 0.1 lap).
- `TimerSetup.LapLimit` is also in tenths; setting `M 1.5` stores `15` and will cause shutdown when `LapCount >= 15`.
- If `LapLimit >= 1.0` (10 tenths) and the system is in `FLY`, it will transition to `BURP` one full lap (10 tenths) before the limit to perform a final burp.

## posTrim (Pitch Trim)

- Computed in `gyro.cpp` (function `speedGyro()`), `posTrim` adjusts throttle based on pitch angle `iangleX`.
- Uses a sin-table mapping for smooth response and inverts sign so that nose-up reduces throttle and nose-down increases throttle.
- Controlled by `TimerSetup.px` (set via menu `E`). Typical tuning: start small (5–10) and observe `P  Pos trim` telemetry.

## maneuverBoost

- `maneuverBoost` is a positive throttle boost based on absolute pitch magnitude and the `TimerSetup.rx` gain (set via menu `K`).
- Small deadband prevents jitter. Used to add temporary throttle during aggressive maneuvering.

## Error Conditions & Safety Cutoffs

The Cheesehead Timer actively monitors flight dynamics, lap counts, and hardware state to protect the aircraft and power system.

### 🛡️ Motion & Gyro Crash Cutoffs
| Error Condition | Trigger Criterion | Active States | System Action & Behavior |
| :--- | :--- | :--- | :--- |
| **Excessive Pitch (`PitchEX`)** | Pitch delta (`dx`) exceeds `PitchExThresh` (default 40°) for `PITCH_SET_N` consecutive samples. | All active states | • Sets `PitchEX = true`<br>• Clears `run_state = false`<br>• Instantly cuts ESC throttle to `0`<br>• Sets Red LED solid ON |
| **Excessive Yaw Rate (`YawEX`)** | Yaw rate (`deg/sec`) exceeds `YawRateExThresh` (default 20°/s) for `YAW_HIGH_N` consecutive samples. Indicates spin-out or line break. | `TAKEOFF`, `FLY` | • Sets `YawEX = true`<br>• Clears `run_state = false`<br>• Instantly cuts ESC throttle to `0` |
| **Abnormally Low Yaw (`YawLOW`)** | Absolute yaw angle drops below `YAW_LOW_THRESHOLD` during flight. Indicates loss of line tension or plane stopping in circle. | `FLY` | • Sets `YawLOW = true`<br>• Clears `run_state = false`<br>• Instantly cuts ESC throttle to `0` |
| **Stalled Takeoff Yaw (`yawSlowCount`)** | Aircraft fails to gain required yaw rotation speed during takeoff within `YAW_SLOW_THRESHOLD` time. | `TAKEOFF` | • Triggers `YawEX`<br>• Clears `run_state = false`<br>• Cuts ESC throttle to `0` |

> [!NOTE]
> **Safety Override (`DS3` DIP Switch)**: Setting DIP switch `DS3` to **LOW** temporarily disables flight-phase yaw checks (`YawEX` and `YawLOW`) during `FLY` state. This is useful for bench testing or manual flight override.

### ⏱️ Lap Limit & Timed Flight Termination
* **Lap Limit Exceeded**: When `LapCount >= LapLimit` (if `LapLimit > 0`), the timer prints `"Lap limit exceeded - shutting down"`, sets `run_state = false`, sets ESC throttle to `0`, and turns off all LEDs.
* **Flight Duration Expiry**: When the `FlyTime` timer expires, the state machine safely progresses through `RDYLAND` and `RAMPDWN` (ramping throttle to `0`) before disabling `run_state`.

### ⚙️ Hardware & Memory Fault Recovery
* **Uninitialized / Corrupt EEPROM**: On initial boot or corrupted memory (when `FlySpeed[2] > 500`), the board auto-recovers by writing factory default parameters to EEPROM, running MPU gyro calibration (`setUpMPU()`), and logging `"First run detected - running MPU calibration"`.
* **I2C / MPU6050 Disconnection**: Loose wiring or sensor disconnection causes MPU read failures, freezing sensor telemetry.

## LED Status Indicators & Signaling

The Cheesehead Timer uses three onboard LEDs to communicate system initialization, real-time flight state machine phases, and diagnostic/error conditions:

### LED Hardware Mapping
| LED Identifier | Software Object | AVR Pin (Nano/Uno) | ESP32 Pin | Indicator Color |
| :--- | :--- | :---: | :---: | :--- |
| **LED3** | `gled` | Pin 7 | GPIO 25 | Green |
| **LED4** | `yled` | Pin 8 | GPIO 26 | Yellow |
| **LED5** | `rled` | Pin 9 | GPIO 27 | Red |

### Flight State Machine LED Patterns
| Flight State | Red LED (`LED5`) | Yellow LED (`LED4`) | Green LED (`LED3`) | Indication / Meaning |
| :--- | :--- | :--- | :--- | :--- |
| **Boot / Init** | Blinking (500ms) | Initialization pattern | Initialization pattern | Board startup & MPU calibration |
| **WAIT** | Slow Blink (250ms ON, 1000ms OFF) | OFF | OFF | Idle / Waiting for arming button |
| **ARMED** | OFF | Slow Blink (250ms ON, 1000ms OFF) | OFF | Armed; countdown to spin-up |
| **TAKEOFF_RAMP** | OFF | Solid ON | Proportional Blink (Flash rate scales with motor throttle) | Throttle ramping up to takeoff |
| **TAKEOFF** | OFF | OFF | Proportional Blink (Flash rate scales with motor throttle) | Full power takeoff phase |
| **FLY** | OFF | Sync Blink (500ms) | Proportional Blink (500ms @ min speed → 25ms @ max speed) | Cruise flight mode (gyro active) |
| **BURP** | Solid ON | Solid ON | Rapid Strobe (25ms strobe @ max speed) | Pre-shutdown throttle burst |
| **RAMPDWN** | Slow Blink (250ms ON, 1000ms OFF) | OFF | OFF | Ramp down / landing phase |
| **Shutdown / Error** | Solid ON | OFF | OFF | Emergency shutdown or safety trigger |

### Manual Terminal Commands
* **`O`** (capital 'O'): Turns **ALL 3 LEDs ON** manually (useful for bench testing hardware connections).
* **`o`** (lowercase 'o'): Turns **ALL 3 LEDs OFF** manually.

## User Button Controls

The primary user push button (connected to `BUTTONPIN`: Pin 10 on AVR Nano / GPIO 33 on ESP32) controls the execution state machine using the `OneButton` debouncing library:

| Gesture / Action | System Condition | Result / Function |
| :--- | :--- | :--- |
| **Single Tap (Click)** | Stopped (`run_state = false`) | **Start Flight Sequence**: Sets `run_state = true` and triggers the state machine starting at the `WAIT` state (1-sec initial burp). |
| **Single Tap (Click)** | Running (`run_state = true`) | **Emergency Stop / Abort**: Instantly stops the run (`run_state = false`), sets ESC throttle to `0`, and returns to `STOPPED` state. |
| **Press & Hold (Long Press)** | Stopped (`run_state = false`) | **Direct Start**: Holding and releasing the button transitions directly into `RUN` mode (`run_state = true`). |
| **Double Click** | Any state | Reserved gesture callback (`handleDoubleClick`). |

## Serial Telemetry Stream

The Cheesehead Timer includes a high-speed, non-blocking serial telemetry stream (19200 baud) for live diagnostics, sensor verification, and desktop GUI interface integration (`desktop_app.py`).

### Toggling Telemetry
* Send **`!`** in the serial terminal to toggle real-time telemetry **ON** or **OFF**.
* Send **`q`** or **`Q`** at any time to turn telemetry **OFF** and return to menu commands.

### Telemetry Line Format
When active, the board streams data lines prefixed with `!` at high frequency:

```text
! speed:120 pitch:2 roll:0 yaw:15 boost:5 leds:R0 Y1 G1 btn:0 state:4
```

### Telemetry Data Fields
| Field Name | Example Value | Description / Units |
| :--- | :--- | :--- |
| **`speed`** | `speed:120` | Current commanded ESC throttle output (`curThrottle`, range `0`..`180`). |
| **`pitch`** | `pitch:2` | Current MPU6050 Pitch angle (`iangleX` in degrees). |
| **`roll`** | `roll:0` | Current MPU6050 Roll angle (`iangleY` in degrees). |
| **`yaw`** | `yaw:15` | Current MPU6050 Yaw angle (`iangleZ` in degrees). |
| **`boost`** | `boost:5` | Active maneuver throttle boost (`maneuverBoost` added to ESC). |
| **`leds`** | `leds:R0 Y1 G1` | Hardware LED states: **R**ed (`LED5`), **Y**ellow (`LED4`), **G**reen (`LED3`) (`0`=OFF, `1`=ON). |
| **`btn`** | `btn:0` | User push button state (`0`=Released, `1`=Pressed). |
| **`state`** | `state:4` | Active flight state machine phase ID (`0`=WAIT, `1`=ARMED, `2`=TAKEOFF_RAMP, `3`=TAKEOFF, `4`=FLY, `5`=BURP, `6`=RDYLAND, `7`=RAMPDWN). |

### Desktop App Integration
The Python GUI (`python desktop_app.py`) parses these exact telemetry lines to update real-time graphs, state badges, LED indicators, and pitch/yaw telemetry displays.

## Tests & Tuning (short)

- Run through `tests.txt` for an ordered test plan (lap counting, BURP behavior, EEPROM persistence, LEDs and DIP switches).
- Tune `px` (`E`) and `rx` (`K`) incrementally while watching `P` and `B` telemetry.

## Troubleshooting

- Compile errors:
  - Ensure `arduino-cli` has AVR core installed.
  - Use the correct FQBN (e.g., `arduino:avr:nano`).
- Duplicate symbol/linker errors:
  - Remove duplicate source files (there was a `state_macine.cpp` duplicate; keep only `state_machine.cpp`).
- Unexpected shutdowns:
  - Check `DS3` position, pitch/yaw thresholds, and `LapLimit`.

## Files to Inspect for Advanced Tuning

- `gyro.cpp` — lap logic, `posTrim`, `maneuverBoost`
- `state_machine.cpp` — throttle application and `setSpeedState()`
- `cheesehead_timer.h` — configuration struct `param` and constants
- `term.cpp` — menu UI and serial parsing

## Contact / Attribution

Original project and author information is present in the code headers (see `term.cpp` banner).

---

File saved as `manual.md` in the project root.
