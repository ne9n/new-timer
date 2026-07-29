# 🧪 Cheesehead Timer — Bench Integration Test Guide

This document provides a comprehensive, step-by-step **Bench Integration Test Plan** for validating the Cheesehead Timer board, sensors, state machine, and throttle outputs **safely on a workbench without flying an airplane**.

---

## ⚠️ Critical Safety Rules Before Starting

> [!CAUTION]
> **REMOVE ALL PROPELLERS** or disconnect the main motor power lead before connecting batteries or testing PWM outputs.
> * Use a **Servo Tester**, **Oscilloscope**, **Multimeter**, or a small **Servo Motor** connected to the `SERVO` output pin instead of a powered drive motor.
> * If an ESC is connected with a motor, **DISCONNECT AT LEAST TWO MOTOR LEADS** or remove the prop.

---

## 🛠️ Required Equipment & Tools

* **Cheesehead Timer Hardware**: Arduino Nano/Uno/ESP32 board, MPU6050 IMU, 3x DIP switches, user button, status LEDs.
* **Power & Data**: USB cable connected to PC.
* **Monitoring Tools**:
  * Serial Terminal software (Arduino Serial Monitor, PuTTY, `screen`) set to **19200 Baud**.
  * Optional: **Desktop GUI App** (`python desktop_app.py`).
  * Signal measurement device (Servo signal reader, oscilloscope, or micro servo).
* **Test Mount**: Flat tabletop or rotating turntable (e.g., a lazy susan or manual swivel stand) for clean MPU6050 rotation.

---

## 📐 Pre-Test Bench Setup

1. Connect the board via USB to your host PC.
2. Secure the MPU6050 sensor board flat on your workbench or test fixture.
3. Open your serial terminal software at **19200 baud** (8-N-1).
4. Type `?` and press **Enter** to load the diagnostic menu.

---

## 📋 Bench Test Suites

### Test Suite 1: Power-On & Boot Verification

| Step | Action | Expected Result | Pass/Fail |
| :--- | :--- | :--- | :---: |
| 1.1 | Plug in USB / Apply 5V power | Board power LED illuminates; status LEDs run initial boot blink sequence. | [ ] |
| 1.2 | Observe Serial Terminal output | Terminal prints initialization banner ending in `init complte`. | [ ] |
| 1.3 | Send `?` command in terminal | Full system diagnostic menu prints with current settings & telemetry. | [ ] |
| 1.4 | Send `O` command (LEDs ON) | `LED3`, `LED4`, and `LED5` light up. | [ ] |
| 1.5 | Send `o` command (LEDs OFF) | `LED3`, `LED4`, and `LED5` turn off. | [ ] |

---

### Test Suite 2: MPU6050 Gyro & Accelerometer Calibration

| Step | Action | Expected Result | Pass/Fail |
| :--- | :--- | :--- | :---: |
| 2.1 | Keep board completely motionless on flat surface | Serial menu telemetry shows stable pitch/yaw angles without drift. | [ ] |
| 2.2 | Send `G` command | Board runs automated calibration routine; offset values compute & display. | [ ] |
| 2.3 | Send `s` command | Calibrated offsets save to EEPROM (`TimerSetup.calX/Y/Z`). | [ ] |
| 2.4 | Power-cycle board and check menu | Calibration values persist after reboot. | [ ] |

---

### Test Suite 3: Physical Axis Mapping (Pitch, Roll, Yaw)

The board allows 360° orientation. Verify physical axes match software configuration:

1. Send `1 0` (Set Pitch = X), `2 1` (Set Roll = Y), `3 2` (Set Yaw = Z) or your required mapping.
2. **Pitch Test**: Tilt nose up 30°. Observe `angleX` / Pitch reading increases smoothly.
3. **Roll Test**: Tilt right wing down 30°. Observe `angleY` / Roll reading changes.
4. **Yaw Test**: Rotate board horizontally 90° clockwise. Observe Yaw reading accumulates correctly.
5. Send `s` to save verified axis mappings.

---

### Test Suite 4: Flight Sequence & State Machine Dry-Run

Simulate a full flight sequence using the button and DIP switches:

```
[WAIT] ──(1s)──> [ARMED] ──(ArmTime)──> [TAKEOFF_RAMP] ──(accelTime)──> [TAKEOFF] ──(5s)──> [FLY]
                                                                                            │
[RAMPDWN] <──(3s)── [RDYLAND] <──(2s)── [BURP] <──(LapLimit - 1.0)──────────────────────────┘
```

| Step | Action | Expected Result | Pass/Fail |
| :--- | :--- | :--- | :---: |
| 4.1 | Set DIP switch `DS1`=ON, `DS2`=OFF | Profile 1 selected (`FlySpeed[0]`). | [ ] |
| 4.2 | Press User Push Button | System enters `WAIT` state (short throttle pulse/burp `curThrottle=25`). | [ ] |
| 4.3 | Wait 1 second | System enters `ARMED` state (`curThrottle=0`). | [ ] |
| 4.4 | Wait `ArmTime` duration | Enters `TAKEOFF_RAMP` state; PWM output ramps smoothly up to max. | [ ] |
| 4.5 | Wait `accelTime` duration | Enters `TAKEOFF` state (`curThrottle = MAX_SPEED`). | [ ] |
| 4.6 | Wait 5 seconds | Enters `FLY` state (`curThrottle = FlySpeed`). | [ ] |
| 4.7 | Let `FlyTime` expire (or trigger via Lap Limit) | Enters `BURP` (throttle pulse), `RDYLAND`, then `RAMPDWN` down to 0. | [ ] |

---

### Test Suite 5: Motion-Aware Throttle (`posTrim` & `maneuverBoost`)

Test dynamic throttle response on the bench during the `FLY` phase:

1. **Pitch Compensation (`posTrim`)**:
   * Set Pitch Gain `E 30`.
   * Trigger state machine into `FLY` state.
   * **Nose UP tilt (30°)**: Observe `P  Pos trim` goes negative. Servo PWM output **decreases** (cuts engine power in climbs).
   * **Nose DOWN tilt (30°)**: Observe `P  Pos trim` goes positive. Servo PWM output **increases** (boosts engine power in dives).
2. **Maneuver Boost (`maneuverBoost`)**:
   * Set Maneuver Gain `K 50`.
   * Perform sharp pitch maneuvers. Observe `B  Maneuver boost` increases and adds positive throttle burst during aggressive pitch changes.
   * Return to level pitch (< 5°): Observe `maneuverBoost` drops back to 0 (deadband active).

---

### Test Suite 6: Simulated Lap Counting & Pre-BURP Shutdown

Simulate flight around a circle by rotating the board on the bench:

| Step | Action | Expected Result | Pass/Fail |
| :--- | :--- | :--- | :---: |
| 6.1 | Send `l` command | `LapCount` resets to `0.0`. | [ ] |
| 6.2 | Manually rotate board horizontally by ~36° (yaw) | `LapCount` increments by `0.1` lap (1 tenth). | [ ] |
| 6.3 | Complete 10 rotation steps (360° total rotation) | `LapCount` reads `1.0` lap in terminal telemetry. | [ ] |
| 6.4 | Set Lap Limit `M 2.0` | `LapLimit` set to `2.0` (20 tenths). | [ ] |
| 6.5 | Enter `FLY` state and rotate board to `1.0` lap | Terminal logs *"Approaching lap limit - switching to BURP"*. Throttle pulses to `BurpMax`. | [ ] |
| 6.6 | Continue rotation to `2.0` laps | Terminal logs *"Lap limit exceeded - shutting down"*. System clears `run_state`, cuts PWM to 0, LEDs turn off. | [ ] |

---

### Test Suite 7: Safety & Crash Cutoff Detection

Validate emergency auto-shutdown features on the bench:

1. **Excessive Pitch Cutoff**:
   * Set pitch threshold low for testing: `h 20`.
   * Start `FLY` state. Tilt board past 20° sharply.
   * **Verify**: Board instantly logs `PitchEX` cutoff, sets `run_state = false`, and drops throttle output to 0 PWM.
2. **Excessive Yaw Rate Cutoff**:
   * Set yaw rate threshold low: `j 20`.
   * Start `FLY` state. Spin board rapidly on yaw axis.
   * **Verify**: Board instantly logs `YawEX` cutoff and cuts throttle.
3. **Safety Override (`DS3` DIP Switch)**:
   * Flip `DS3` switch to **LOW**.
   * Repeat excessive yaw test during `FLY`.
   * **Verify**: Safety checks are bypassed when `DS3` is LOW (used for test/override mode).

---

### Test Suite 8: Desktop GUI Application Bench Test

1. Launch python GUI on host PC:
   ```powershell
   python desktop_app.py
   ```
2. Select COM port and baud rate `19200`, click **Connect**.
3. **Verify GUI Telemetry**:
   * Real-time charts/indicators for `LapCount`, `posTrim`, `maneuverBoost`, and active Flight State.
   * Parameters modified in GUI update on the board and persist after sending **Save**.

---

## 📊 Integration Test Sign-Off Checklist

| Test Suite | Area Covered | Tester Initials | Status (PASS / FAIL) | Date |
| :--- | :--- | :---: | :---: | :---: |
| **Suite 1** | Power-On & Serial Terminal | | | |
| **Suite 2** | Gyro Calibration & EEPROM | | | |
| **Suite 3** | Axis Orientation Mapping | | | |
| **Suite 4** | Flight State Machine Dry-Run | | | |
| **Suite 5** | `posTrim` & `maneuverBoost` | | | |
| **Suite 6** | Lap Counting & Pre-BURP Cutoff | | | |
| **Suite 7** | Crash Safety Shutdown & DIP Overrides | | | |
| **Suite 8** | Python Desktop GUI Integration | | | |

---

## 🔍 Troubleshooting Bench Anomalies

* **Servo/PWM output jitter**: Verify common ground (GND) between Arduino, ESC/servo tester, and power supply.
* **Gyro Drift**: Run `G` calibration command on a completely stationary table.
* **No Serial Output**: Ensure baud rate is set to **19200** (not 9600 or 115200).
* **Lap count not incrementing**: Verify Yaw axis assignment using command `3` in serial menu.
