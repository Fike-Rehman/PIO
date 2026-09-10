#include <Arduino.h>

namespace {
constexpr unsigned long heartbeatIntervalMs = 5000;
unsigned long previousHeartbeatMs = 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ENCELADUS");
  Serial.println("Firmware: 0.0.1 - Phase 0");
  Serial.println("Board target: nodemcu-32s (reused Ariel ESP32)");
  Serial.println("Boot OK - serial diagnostics ready");
}

void loop() {
  const unsigned long now = millis();
  if (now - previousHeartbeatMs >= heartbeatIntervalMs) {
    previousHeartbeatMs = now;
    Serial.printf("ENCELADUS alive | uptime_ms=%lu | free_heap=%lu\n",
                  now, static_cast<unsigned long>(ESP.getFreeHeap()));
  }
  yield();
}
