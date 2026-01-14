# Cheesehead Timer — User Manual & Setup Guide

## Overview

This project (`cheesehead_timer`) is a control-line throttle sequencer for model aircraft. It manages arming, takeoff ramp, flight timing, BURP (short throttle burst), lap counting (tenths-of-a-lap), and safety shutdowns based on pitch/yaw detection.

Files of interest:
- `cheesehead_timer.ino` — main sketch and setup/loop
- `gyro.cpp` / `gyro.h` — MPU6050 reads, lap counting, `posTrim`, `maneuverBoost`, safety checks
- `state_machine.cpp` / `state_machine.h` — state transitions and throttle output
- `term.cpp` / `term.h` — serial menu for configuration and telemetry
- `doc.txt`, `manual.md`, `tests.txt` — documentation and test plan

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

## Safety & Crash Detection

- Hysteresis counters and thresholds (`PitchExThresh`, `YawRateExThresh`) prevent noisy false triggers.
- On excessive pitch or yaw detection the code sets `run_state = false` and cuts throttle.
- `DS3` can be used (when LOW) to temporarily disable some flight-phase checks during `FLY`.

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
