/*****************
Fike Rehman
4/28/2023

Ariel:

  - Runs a web server on the ESP32 board that listens for incoming requests
    and hosts the following API:

    - /Ping: Pings the server and returns a "Ping OK" response
    - /Lights: Toggles the lights on and off
    - /Time: Returns the current time in Minneapolis
    - /Status: Returns the current status of the lights (on or off)

*****************/

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi Definitions
const char WiFiSSID[] = "OCEANLAB";
const char WiFiPSK[] = "FikeNETBB1972";

const int LEDPIN = 2;
const int MOSFET_GATE_PIN = 4;
const int PUSH_BUTTON = 15;

int buttonReading = 0;
int lastButtonReading = 0;

boolean ledOn = false;

WiFiServer server(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void connectWiFi()
{
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP32
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected");
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  connectWiFi();
  server.begin();

  timeClient.begin();
  // set time offset in seconds to adjust for your timezone, Central Standard Time is UTC -5 hours
  timeClient.setTimeOffset(-18000);

  pinMode(LEDPIN, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLDOWN);
  pinMode(MOSFET_GATE_PIN, OUTPUT);

  // set the LED to off when starting
  digitalWrite(LEDPIN, LOW);
  int ledOn = false;
  digitalWrite(MOSFET_GATE_PIN, LOW);
}

// toggles the shelf lights on and off (controls both the LED Pin and the MOSFET gate)
void toggleLights()
{
  if (ledOn)
  {
    digitalWrite(LEDPIN, LOW);
    ledOn = false;
    digitalWrite(MOSFET_GATE_PIN, LOW);
  }
  else
  {
    digitalWrite(LEDPIN, HIGH);
    ledOn = true;
    digitalWrite(MOSFET_GATE_PIN, HIGH);
  }
}

void MonitorButtonPress()
{
  buttonReading = digitalRead(PUSH_BUTTON);

  if (buttonReading != lastButtonReading)
  {
    if (buttonReading == HIGH)
    {
      Serial.println("Button Pressed");
      toggleLights();
    }
    delay(50);
  }
  lastButtonReading = buttonReading;
}

void loop()
{
  timeClient.update();
  MonitorButtonPress();

  WiFiClient client = server.available(); // Listen for incoming clients
  if (!client)
  {
    return;
  }

  Serial.println("Web Server Started!");

  // Prepare the response
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: text/html\r\n\r\n";
  response += "<!DOCTYPE HTML>\r\n<html>\r\n";
  response += "<h1>Welcome to Ariel</h1>\r\n";

  String request = client.readStringUntil('\r'); // read a byte, then
  Serial.println("Incoming request");            // print it out the serial monitor
  Serial.println(request);                       // print it out the serial monitor

  // Match the request
  if (request.indexOf("/Ping") != -1)
  { // Ping request
    response += "Ping OK\r\n";
  }
  else if (request.indexOf("/Lights") != -1)
  { // Lights toggle request
    toggleLights();
    response += "Lights toggled " + String(ledOn ? "ON" : "OFF") + " at " + timeClient.getFormattedTime() + "\r\n";
  }
  else if (request.indexOf("/Time") != -1)
  { // Time request
    response += "Current Time in Minneapolis is: " + timeClient.getFormattedTime() + "\r\n";
  }
  else if (request.indexOf("/Status") != -1)
  { // Status request
    response += "Lights are " + String(ledOn ? "ON" : "OFF") + "\r\n";
  }
  else
  {
    response += "Unknown request\r\n";
  }

  response += "</html>\r\n";
  client.print(response);
  delay(1);
  Serial.println("Client disonnected");
}
