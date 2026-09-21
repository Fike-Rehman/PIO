#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "DisplayConfig.h"
#include "DisplayManager.h"

namespace {
  Adafruit_SSD1306 display(DisplayConfig::width, DisplayConfig::height, &Wire, -1);
  bool oledReady = false;

  // Thicken the built-in small font by one pixel, leaving space between letters.
  void drawSmallBoldText(const char* text, int y) {
    int x = 0;
    while (*text != '\0') {
      const unsigned char character = static_cast<unsigned char>(*text++);
      display.drawChar(x, y, character, SSD1306_WHITE, SSD1306_WHITE, 1);
      display.drawChar(x + 1, y, character, SSD1306_WHITE, SSD1306_WHITE, 1);
      x += 7;
    }
  }

  void setupDisplay() {
    Serial.printf("I2C: SDA=%d SCL=%d | SSD1306 %dx%d\n",
                  DisplayConfig::sdaPin, DisplayConfig::sclPin,
                  DisplayConfig::width, DisplayConfig::height);
    Wire.begin(DisplayConfig::sdaPin, DisplayConfig::sclPin);
    Wire.setTimeOut(50);
    uint8_t oledAddress = 0;
    unsigned int oledAddressMatchCount = 0;
    Serial.println("Scanning I2C bus...");
    for (uint8_t address = 1; address < 127; ++address) {
      Wire.beginTransmission(address);
      if (Wire.endTransmission() == 0) {
        Serial.printf("I2C device: 0x%02X\n", address);
        if (address == 0x3C || address == 0x3D) {
          oledAddress = address;
          ++oledAddressMatchCount;
        }
      }
    }
    if (oledAddressMatchCount != 1) {
      Serial.println(oledAddressMatchCount == 0
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
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    display.cp437(true); // Use the correct extended-font character mapping.
    // The 128x64 panel's top 16 pixels are yellow; the lower region is cyan.
    display.setTextSize(DisplayConfig::height == 64 ? 2 : 1);
    display.setCursor(0, 0);
    display.print("ENCELADUS");

    // Static Phase 1 layout samples, not live sensor measurements.
    display.setTextSize(1);
    // CP437 byte 0xF8 is the degree symbol; split literals end the hex escape.
    const char* lines[] = {"TEMP: 21 \xF8" "C", "FLOW RATE: 1.45L", "LEVEL: Low"};
    const int firstRowY = DisplayConfig::height == 64 ? 20 : 8;
    const int rowSpacing = DisplayConfig::height == 64 ? 16 : 8;
    for (int row = 0; row < 3; ++row) {
      drawSmallBoldText(lines[row], firstRowY + row * rowSpacing);
    }
    display.display();
    oledReady = true;
    Serial.printf("OLED: initialized at 0x%02X. Confirm all four lines visually.\n", oledAddress);
  }
}

namespace DisplayManager {
  void begin() {
    oledReady = false;
    setupDisplay();
  }

  bool isReady() {
    return oledReady;
  }
}
