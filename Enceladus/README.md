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
TEMP: 21 °C
FLOW RATE: 1.45L
LEVEL: Low
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

The user verified successful startup and power-cycle recovery. Replacing the
original panel resolved the uneven brightness observed on that panel.

The current 128x64 layout keeps ENCELADUS at size 2 (12x16 character cells)
in the yellow top region. The three sample rows use size 1 with a one-pixel bold overdraw and
seven-pixel character spacing,
starting at y=20, 36, and 52 in the cyan region. Values are static layout
samples, not sensor measurements; no sensor or WLED functionality is included.
The user approved the bold cyan rows on the replacement panel. The temperature
sample now includes a degree symbol (CP437 0xF8); its appearance awaits confirmation.

The I2C scan count is named `oledAddressMatchCount` for clarity.
## Code organization

- `src/main.cpp`: serial startup, display initialization call, and heartbeat.
- `include/DisplayManager.h`: public `begin()` and `isReady()` declarations.
- `src/DisplayManager.cpp`: private OLED instance, I2C scan, and screen rendering.
- `include/DisplayConfig.h`: wiring and panel dimensions.

The display implementation stays in one source file, so including the public
header does not create additional OLED objects. This separation preserves the
existing layout, sample values, degree symbol, and bold cyan rows.
