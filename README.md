# Cheesehead Timer

A control-line airplane throttle sequencer with MPU6050 gyroscope flight state tracking, pitch dynamic trim, and lap counting.

---

## Project Documentation Index

All project documentation is located in the [`docs/`](docs/) directory:

* [**Gyroscope Operation & Control**](docs/GYRO_OPERATION.md) — Comprehensive guide to MPU6050 initialization, baseline locking, dynamic trim, start/stop motion handling, and crash safety cutoffs.
* [**Operation Manual**](docs/operation_manual.md) — Operational document covering pinouts, terminal menu commands, profile configurations, and state machine overview.
* [**Arduino Flowchart & Architecture**](docs/arduino_flowchart.md) — Architectural diagrams and execution flowcharts for the firmware.
* [**User Manual**](docs/manual.md) — Instructions for setting up and operating the Cheesehead Timer.
* [**Project Overview**](docs/project_overview.txt) — Summary of project hardware target and speed control math formulas.
* [**Detailed Tech Reference**](docs/doc.txt) — In-depth technical reference notes.

---

## Directory Structure

* `cheesehead_timer/` — Main C++/Arduino firmware (`cheesehead_timer.ino`, `gyro.cpp`, `state_machine.cpp`, `term.cpp`).
* `docs/` — All technical and operational documentation.
* `boards/` & `platformio.ini` — PlatformIO build configurations.
