#!/usr/bin/env pwsh
# Run PlatformIO tests on-device for the `nano` environment.
# Make sure PlatformIO Core is installed and your board is connected.

# Optionally update COM port in platformio.ini (upload_port) before running.
pio test -e nano
