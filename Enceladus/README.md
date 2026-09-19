# Enceladus firmware - Phase 1 OLED

Phase 0 hardware acceptance is complete: the user confirmed the environment
works and the ESP32 can be flashed. Board target remains `nodemcu-32s`.

## Display configuration

SSD1306 I2C OLED: SDA on GPIO21, SCL on GPIO22, and common GND.
Verify the module's supply requirements and use ESP32-compatible 3.3 V I2C levels.
`include/DisplayConfig.h` contains the pins and resolution. The initial resolution
is 128x64; change `height` to 32 if your panel is 128x32. Resolution cannot be
identified by the I2C scan. The user has confirmed the SSD1306 controller.

At startup the firmware scans I2C and reports every responding address over
serial. It initializes a single OLED found at 0x3C or 0x3D. No matching address
or two matching addresses produces a diagnostic instead of choosing arbitrarily.

## Build and upload

From the Enceladus directory:

```powershell
pio run
pio run --target upload
pio device monitor --baud 115200
```

Alternatively, select Enceladus in the existing PlatformIO VS Code workspace
and use Build, Upload, and Serial Monitor. Select a port explicitly if multiple
serial devices are connected. Close other serial monitors before uploading.

## Hardware acceptance

Press EN/reset with the serial monitor open. Confirm the detected I2C address
and visually check these four lines:

```text
ENCELADUS
PHASE 1
OLED OK
READY
```

The Phase 0 serial heartbeat continues every five seconds. The OLED screen is
static; the heartbeat confirms the firmware is still running. The serial
`initialized` message only confirms software initialization, not visible output
or continued display connectivity.

- Confirm all four lines are readable and correctly spaced.
- Repeat reset and power-cycle checks and confirm the screen returns reliably.
- If no OLED is detected, check supply, ground, wiring and configured pins.
- If the device is detected but the image is blank or malformed, verify the
  panel resolution and module power requirements.

The user verified the initial screen is correct and returns after power cycling.
The 128x64 layout now uses double-size text (12x16 character cells), with four
16-pixel rows. The user confirmed improved readability but reported fixed uneven
brightness, particularly in the lower three rows. Hardware diagnosis is pending. After acceptance,
commit/checkpoint this phase before beginning Phase 2. No sensors or WLED
functionality have been added.

## Display diagnosis status

The temporary diagnostic was removed and the large-text Phase 1 screen restored.
The user reported that stripes looked OK, but letters still had uneven brightness
in the same areas. A replacement panel comparison is planned; a panel fault is
not yet confirmed.
