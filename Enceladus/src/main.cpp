#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "DisplayConfig.h"

namespace {
constexpr unsigned long heartbeatIntervalMs = 5000;
unsigned long previousHeartbeatMs = 0;
Adafruit_SSD1306 display(DisplayConfig::width, DisplayConfig::height, &Wire, -1);
bool oledReady = false;

void setupDisplay() {
  Serial.printf("I2C: SDA=%d SCL=%d | SSD1306 %dx%d\n",
                DisplayConfig::sdaPin, DisplayConfig::sclPin,
                DisplayConfig::width, DisplayConfig::height);
  Wire.begin(DisplayConfig::sdaPin, DisplayConfig::sclPin);
  Wire.setTimeOut(50);
  uint8_t oledAddress = 0;
  unsigned int candidates = 0;
  Serial.println("Scanning I2C bus...");
  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("I2C device: 0x%02X\n", address);
      if (address == 0x3C || address == 0x3D) {
        oledAddress = address;
        ++candidates;
      }
    }
  }
  if (candidates != 1) {
    Serial.println(candidates == 0
        ? "OLED: NOT FOUND at 0x3C/0x3D. Check power, ground, SDA and SCL."
        : "OLED: ambiguous addresses. Connect only one OLED for Phase 1.");
    return;
  }
  // Preserve explicit I2C pins; the four-pin module has no reset pin.
  if (!display.begin(SSD1306_SWITCHCAPVCC, oledAddress, false, false)) {
    Serial.println("OLED: framebuffer allocation failed");
    return;
  }
  display.clearDisplay();
  // Use 12x16 character cells on the 128x64 panel for better readability.
  display.setTextSize(DisplayConfig::height == 64 ? 2 : 1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  const char* lines[] = {"ENCELADUS", "PHASE 1", "OLED OK", "READY"};
  for (int row = 0; row < 4; ++row) {
    display.setCursor(0, row * (DisplayConfig::height / 4));
    display.print(lines[row]);
  }
  display.display();
  oledReady = true;
  Serial.printf("OLED: initialized at 0x%02X. Confirm all four lines visually.\n", oledAddress);
}
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ENCELADUS");
  Serial.println("Firmware: 0.1.1 - Phase 1 OLED large text");
  Serial.println("Board target: nodemcu-32s (reused Ariel ESP32)");
  Serial.println("Boot OK - serial diagnostics ready");
  setupDisplay();
}

void loop() {
  const unsigned long now = millis();
  if (now - previousHeartbeatMs >= heartbeatIntervalMs) {
    previousHeartbeatMs = now;
    Serial.printf("ENCELADUS alive | uptime_ms=%lu | free_heap=%lu | OLED=%s\n",
                  now, static_cast<unsigned long>(ESP.getFreeHeap()),
                  oledReady ? "initialized (verify screen)" : "FAILED");
  }
  yield();
}
