#include <Arduino.h>
#include "DisplayManager.h"

namespace {
constexpr unsigned long heartbeatIntervalMs = 5000;
unsigned long previousHeartbeatMs = 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ENCELADUS");
  Serial.println("Firmware: 0.1.5 - Phase 1 OLED degree symbol");
  Serial.println("Board target: nodemcu-32s (reused Ariel ESP32)");
  Serial.println("Boot OK - serial diagnostics ready");
  Serial.println("Display values are static Phase 1 samples, not sensor readings.");
  DisplayManager::begin();
}

void loop() {
  const unsigned long now = millis();
  if (now - previousHeartbeatMs >= heartbeatIntervalMs) {
    previousHeartbeatMs = now;
    Serial.printf("ENCELADUS alive | uptime_ms=%lu | free_heap=%lu | OLED=%s\n",
                  now, static_cast<unsigned long>(ESP.getFreeHeap()),
                  DisplayManager::isReady() ? "initialized (verify screen)" : "FAILED");
  }
  yield();
}
