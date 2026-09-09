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
void InitTime();
void connectWifi();
void handleMOSFET(AsyncWebServerRequest *request, int pin, bool state, String mosfetName);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -21600, 60000); // UTC -6 hours for Minneapolis, update every 60 seconds

// ==== HTTP Server ====
AsyncWebServer server(80);

 // ==== GPIO ====
const int MOSFET_PIN_1 = D1;  // GPIO pin to control the MOSFET (D1 on NodeMCU)
const int MOSFET_PIN_2 = D2;  // GPIO pin to control the MOSFET (D2 on NodeMCU)
const int MOSFET_PIN_3 = D5;  // GPIO pin to control the MOSFET (D5 on NodeMCU)

void setup() {
  Serial.begin(115200);
  delay(100);

  connectWifi();

  pinMode(MOSFET_PIN_1, OUTPUT);
  digitalWrite(MOSFET_PIN_1, LOW);  // Start OFF

  pinMode(MOSFET_PIN_2, OUTPUT);
  digitalWrite(MOSFET_PIN_2, LOW);  // Start OFF

  pinMode(MOSFET_PIN_3, OUTPUT);
  digitalWrite(MOSFET_PIN_3, LOW);  // Start OFF

  // --- Endpoints for MOSFET control---
  // M1
  server.on("/M1/on", HTTP_GET, [](AsyncWebServerRequest *request){
    handleMOSFET(request, MOSFET_PIN_1, true, "M1");
  });
  server.on("/M1/off", HTTP_GET, [](AsyncWebServerRequest *request){
    handleMOSFET(request, MOSFET_PIN_1, false, "M1");
  });

  // M2
  server.on("/M2/on", HTTP_GET, [](AsyncWebServerRequest *request){
    handleMOSFET(request, MOSFET_PIN_2, true, "M2");
  });
  server.on("/M2/off", HTTP_GET, [](AsyncWebServerRequest *request){
    handleMOSFET(request, MOSFET_PIN_2, false, "M2");
  });

  // M3
  server.on("/M3/on", HTTP_GET, [](AsyncWebServerRequest *request){
    handleMOSFET(request, MOSFET_PIN_3, true, "M3");
  });
  server.on("/M3/off", HTTP_GET, [](AsyncWebServerRequest *request){
    handleMOSFET(request, MOSFET_PIN_3, false, "M3");
  });

  // Ping endpoint
  server.on("/ping", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "pong");
  });

  // Status endpoint
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
  char status[64];

  snprintf(
    status,
    sizeof(status),
    "M1=%s\nM2=%s\nM3=%s",
    digitalRead(MOSFET_PIN_1) == HIGH ? "ON" : "OFF",
    digitalRead(MOSFET_PIN_2) == HIGH ? "ON" : "OFF",
    digitalRead(MOSFET_PIN_3) == HIGH ? "ON" : "OFF"
  );

  request->send(200, "text/plain", status);
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

void InitTime()
{
  timeClient.begin();

  for (int i = 0; i < 10; i++) {
    timeClient.update();
    if (timeClient.getEpochTime() > 1700000000) {
      break;
    }
    delay(500);
  }

  if (timeClient.getEpochTime() > 1700000000) {
    Serial.print("Current time (CST): ");
    Serial.println(timeClient.getFormattedTime());
  } else {
    Serial.println("Time not yet available");
  }
}

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

  InitTime();  
}

void handleMOSFET(AsyncWebServerRequest *request, int pin, bool state, String mosfetName) {
  digitalWrite(pin, state ? HIGH : LOW);
  
  Serial.printf("Request received for %s -> %s\n", mosfetName.c_str(), state ? "ON" : "OFF");
  String msg = String(mosfetName) + (state ? " ON" : " OFF");
  request->send(200, "text/plain", msg);
}