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
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// ====== Function prototypes =====
void connectWifi();

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 60000); // UTC -5 hours for Minneapolis, update every 60 seconds

// ==== HTTP Server ====
AsyncWebServer server(80);

 // ==== GPIO ====
const int MOSFET_PIN = D1;  // GPIO pin to control the MOSFET (D1 on NodeMCU)


void setup() {
  Serial.begin(115200);
  delay(100);

  connectWifi();

  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);  // Start OFF


  // ==== HTTP endpoints ====
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(MOSFET_PIN, HIGH);
    Serial.println("MOSFET 1 ON");
    request->send(200, "text/plain", "MOSFET ON");
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(MOSFET_PIN, LOW);
    Serial.println("MOSFET 1 OFF");
    request->send(200, "text/plain", "MOSFET OFF");
  });

  // Ping endpoint
  server.on("/ping", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "pong");
  });

    // Status endpoint
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String state = (digitalRead(MOSFET_PIN) == HIGH) ? "ON" : "OFF";
    request->send(200, "application/json", "{\"mosfet\":\"" + state + "\"}");

  });

  // Time endpoint
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
    timeClient.update();
    String currentTime = timeClient.getFormattedTime();
    String message = "Current time: " + currentTime;
    request->send(200, "text/plain", message);
});

  // ==== end Http endpoints ====

  server.begin();
  Serial.println("HTTP server started.");

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