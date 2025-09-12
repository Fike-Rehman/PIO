/*****************
Fike Rehman
9/04/2025

Miranda:

  - Runs a web server on the ESP8266 board that listens for incoming requests
    and hosts the following API.This is same as project Ariel but modified to but
    modified to control the lights on a ESP8266 based board.

    - /Ping: Pings the server and returns a "Ping OK" response
    - /Lights: Toggles the lights on and off
    - /Time: Returns the current time in Minneapolis
    - /Status: Returns the current status of the lights (on or off)

  History:
    

*****************/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <config.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// ====== Function prototypes =====
void connectWifi();

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 60000); // UTC -5 hours for Minneapolis, update every 60 seconds


void setup() {
  Serial.begin(115200);
  delay(100);

 connectWifi();

}

void loop() {
  // put your main code here, to run repeatedly:
}

// ===== Function definitions =====
void connectWifi() {
  Serial.println();
  Serial.println("Booting...");
  Serial.println("Connecting to: " + String(WiFiSSID));

  // Start WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFiSSID, WiFiPSK);

  Serial.print("Connecting to WiFi ");
  Serial.print(WiFiSSID);

  // Wait for connection
  uint8_t retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retries++;
    if (retries > 60) {  // 30 sec timeout
      Serial.println("\nFailed to connect to WiFi!");
      return;
    }
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  // Initialize and get time
  timeClient.begin();
  if (timeClient.forceUpdate()) {
    Serial.print("Current time (CST): ");
    Serial.println(timeClient.getFormattedTime());
  } else {
    Serial.println("Failed to get time from NTP server");
  }
  
}