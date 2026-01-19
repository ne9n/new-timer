Port notes — XIAO nRF52840

This branch (port/xiao-nrf52840) adds conditional pin mappings and an option to use
the board's internal IMU via the `USE_INTERNAL_IMU` compile flag.

Quick build instructions

- Option A — arduino-cli
  - Install the appropriate core for your XIAO board (e.g., `arduino:mbed` or Seeed core).
  - Compile with a build flag to enable the XIAO mapping and internal IMU:

    arduino-cli compile --fqbn <YOUR_FQBN> --build-property build.extra_flags="-DXIAO_NRF52840" <path-to-sketch>

  - Example (replace <YOUR_FQBN> with your board's FQBN):

    arduino-cli compile --fqbn adafruit:nrf52:feather_nrf52840_express --build-property build.extra_flags="-DXIAO_NRF52840" C:\path\to\cheeshead_timer

- Option B — PlatformIO
  - In `platformio.ini` add a build flag for the environment:

    build_flags = -DXIAO_NRF52840

Notes & requirements

- The internal IMU code uses the Arduino `IMU` API (`Arduino_LSM6DS` / `Arduino_LSM6DSC` driver). Ensure the board core or library providing `IMU.*` is available for your target. If not, install the appropriate library or replace the wrapper with the vendor-specific driver.

- The default XIAO pin mapping in `cheesehead_timer.h` is an example. Verify and adjust pin numbers to match your XIAO variant (Seeed XIAO nRF52840 uses different pins depending on the connector footprint).

- If you want me to set specific pin mappings for your exact XIAO model, tell me which variant (Seeed XIAO nRF52840/Seeed Tiny) and I'll update `cheesehead_timer.h` accordingly.

- After building for XIAO, upload with `arduino-cli upload --fqbn <YOUR_FQBN> -p <PORT> <path-to-sketch>` or use PlatformIO/IDE as usual.
