# Enceladus firmware - Phase 0

Uses the exact ESP32 previously deployed in Ariel, with Ariel's `nodemcu-32s`
PlatformIO board target and Arduino framework.

## Build and run

From this directory:

```powershell
pio run
pio run --target upload
pio device monitor --baud 115200
```

Alternatively, open the repository's `PlatformIO.code-workspace` in VS Code and
select the Enceladus project in PlatformIO. If multiple serial devices are
connected, select the board's port explicitly for upload and monitoring.

Firmware prints startup diagnostics at 115200 baud and a heartbeat every five
seconds. Press EN/reset with the serial monitor open to see the startup banner.
No sensors, lighting, or Wi-Fi configuration are required for Phase 0.
Uploading replaces the retired Ariel firmware on the board.

## Hardware acceptance

- Build completes successfully.
- Upload succeeds on the reused Ariel board.
- Reset produces the ENCELADUS startup banner.
- Heartbeats continue and repeated power cycles boot reliably.

Build success alone does not establish hardware acceptance. Complete these
checks on the board before proceeding to Phase 1 (OLED).
