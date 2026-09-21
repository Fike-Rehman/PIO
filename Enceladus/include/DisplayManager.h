#pragma once

namespace DisplayManager {
// Call after Serial.begin(); scans I2C, initializes the OLED, and draws the screen.
void begin();

// Reports initialization success, not ongoing connectivity or visible output.
bool isReady();
}
