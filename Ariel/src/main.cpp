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

  History:
    8/6/2023: Added code to control the Puck lights and the Dream light

*****************/

#include <Arduino.h>
#include <WiFi.h>
#include <config.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <arielPatterns.h>

const int INDICTOR_LED_PIN = 2; // indicater LED
const int MOSFET_GATE1_PIN = 4; // MOSFET that controls the 12v Puck lights
const int MOSFET_GATE2_PIN = 5; // MOSFET that controls 5v the dream light
const int PUSH_BUTTON = 15;

const int LED_STRIP_PIN = 18; // pin for the LED strip

const int MAX_LED_VOLTS = 5;   // Max volts for LED strip
const int MAX_LED_AMPS = 4000; // max current draw in milliamps

int buttonReading = 0;
int lastButtonReading = 0;

boolean lightsOn = false;

WiFiServer server(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Task handles
TaskHandle_t SetLEDPatternTask;

// =============================== connectWifi ========================================
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

// ================================== toggleLights ===================================
// toggles the shelf lights on and off (controls both the LED Pin and the MOSFET gate)
void toggleLights()
{
  if (lightsOn)
  {
    digitalWrite(INDICTOR_LED_PIN, LOW);
    lightsOn = false;
    digitalWrite(MOSFET_GATE1_PIN, LOW);
    digitalWrite(MOSFET_GATE2_PIN, LOW);
  }
  else
  {
    digitalWrite(INDICTOR_LED_PIN, HIGH);
    lightsOn = true;
    digitalWrite(MOSFET_GATE1_PIN, HIGH);
    digitalWrite(MOSFET_GATE2_PIN, HIGH);
  }
}

// ============================== monitorButtonPress =========================================
// monitors the hard button press on the Ariel board to turn on/ off shelf and dream lights
void monitorButtonPress()
{
  buttonReading = digitalRead(PUSH_BUTTON);

  if (buttonReading != lastButtonReading)
  {
    if (buttonReading == HIGH)
    {
      Serial.println("Button Pressed");
      toggleLights();
    }
    delay(100);
  }
  lastButtonReading = buttonReading;
}

// =========================== handleHttpRequest ===============================================
// looks for the incoming HTTP requests on the web server and returns a reponse to each request
void handleHTTPRequest()
{
  WiFiClient client = server.available(); // Listen for incoming clients
  if (!client)
  {
    return;
  }

  Serial.println("Web Server running!");

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
    response += "Lights toggled " + String(lightsOn ? "ON" : "OFF") + " at " + timeClient.getFormattedTime() + "\r\n";
  }
  else if (request.indexOf("/SolidColor?color=") != -1)
  {
    request.replace("%22", "\""); // replace URL encoded double qoutes with actual double qoutes
    int paramIndex = request.indexOf("color=");

    if (paramIndex != -1)
    {
      int colorValueStart = paramIndex + 7; // Length of "color=" is 7, but we add 1 for end double qoutes
      int colorValueEnd = request.indexOf("\"", colorValueStart);

      // If a closing double quote is found
      if (colorValueEnd != -1)
      {
        // Extract the color value
        String colorValue = request.substring(colorValueStart, colorValueEnd);

        // set the LED's to new color value
        ledSolidColor = getColorFromString(colorValue); // set the value for the solid color

        // need to update our shared variable using semaphore
        if (xSemaphoreTake(arielLEDPatternSemaphore, portMAX_DELAY) == pdTRUE)
        {
          arielLEDPattern = STRIP_SOLID_COLOR; // set LED pattern to solid color
          response += "LED's Set to Solid Color: " + colorValue + " at " + timeClient.getFormattedTime() + "\r\n";
          xSemaphoreGive(arielLEDPatternSemaphore);
        }
      }
    }
    else
    {
      response += "LED's Set to Solid Color bad request parameter";
    }
  }
  else if (request.indexOf("/KnightRider") != -1)
  {
    // need to update our shared variable using semaphore
    if (xSemaphoreTake(arielLEDPatternSemaphore, portMAX_DELAY) == pdTRUE)
    {
      arielLEDPattern = STRIP_KNIGHT_RIDER; // set LED pattern to knightRider
      response += "Knight Rider LED effect set at " + timeClient.getFormattedTime() + "\r\n";
      xSemaphoreGive(arielLEDPatternSemaphore);
    }
  }
  else if (request.indexOf("/Time") != -1)
  { // Time request
    response += "Current Time in Minneapolis is: " + timeClient.getFormattedTime() + "\r\n";
  }
  else if (request.indexOf("/Status") != -1)
  { // Status request
    response += "Lights are " + String(lightsOn ? "ON" : "OFF") + "\r\n";
  }
  else
  {
    response += "Unknown request\r\n";
  }

  response += "</html>\r\n";
  client.print(response);
  delay(25);
  Serial.println("Client disonnected");
}

// ========================================= setLEDPattern =============================
// Sets the pattern to run on the Ariel LED strip based on the value of 'arielLEDPattern' variable
// Note: this method is executed on its own task on Core 0.
void setLEDPattern(void *parameter)
{
  LEDPattern currentPattern;

  while (1)
  {
    Serial.print("Running on core: ");
    Serial.println(xPortGetCoreID());

    // need to update our shared variable using semaphore
    if (xSemaphoreTake(arielLEDPatternSemaphore, portMAX_DELAY) == pdTRUE)
    {
      currentPattern = arielLEDPattern; // what is the current value of pattern
      Serial.print("Curent LED Pattern value:");
      Serial.println(currentPattern);

      if (currentPattern == STRIP_OFF)
      {
        setSolidColor(CRGB::Black); // turn the led strip off
      }
      else if (currentPattern == STRIP_SOLID_COLOR)
      {
        setSolidColor(ledSolidColor); // turn the led strip off
      }
      else if (currentPattern == STRIP_KNIGHT_RIDER) // Knight rider pattern requested
      {
        knightRider(ledSolidColor);
      }
      xSemaphoreGive(arielLEDPatternSemaphore);
    }

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// =========================================== setup ===========================================
// The main entry point for the code. Initializes wifi connection, pin modes, leds strip initial
// set up etc. Also creates a new task to control patterns on the Ariel LED strip.
void setup()
{
  Serial.begin(115200);
  connectWiFi();
  server.begin();

  timeClient.begin();
  // set time offset in seconds to adjust for your timezone, Central Standard Time is UTC -5 hours
  timeClient.setTimeOffset(-18000);

  pinMode(INDICTOR_LED_PIN, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLDOWN);
  pinMode(MOSFET_GATE1_PIN, OUTPUT);
  pinMode(MOSFET_GATE2_PIN, OUTPUT);
  pinMode(LED_STRIP_PIN, OUTPUT);

  // set the Puck lights and the Dream light to off when starting
  digitalWrite(INDICTOR_LED_PIN, LOW);
  lightsOn = false;
  digitalWrite(MOSFET_GATE1_PIN, LOW);
  digitalWrite(MOSFET_GATE2_PIN, LOW);

  FastLED.addLeds<WS2812B, LED_STRIP_PIN, GRB>(ariel_LEDs, NUM_LEDS);  // add LED's to the FastLED library
  FastLED.setMaxPowerInVoltsAndMilliamps(MAX_LED_VOLTS, MAX_LED_AMPS); // limit power settings
  FastLED.setBrightness(128);                                          // set the brightness of the LED's

  // create the semaphore for arielLEDPattern variable since it is shared
  // between two tasks:
  arielLEDPatternSemaphore = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
      setLEDPattern,       /* Function to implement the task */
      "SetLEDPatternTask", /* Name of the task */
      2048,                /* Stack size in words */
      NULL,                /* Task input parameter */
      0,                   /* Priority of the task */
      &SetLEDPatternTask,  /* Task handle. */
      0);                  /* Core where the task should run */

  Serial.println("Setup completed.");
}

// =========================================== loop ======================================
// the main loop method.
void loop()
{
  Serial.print("Running on core: ");
  Serial.println(xPortGetCoreID());

  timeClient.update();
  monitorButtonPress();
  handleHTTPRequest();

  // This delay is important. It allows our tasks run
  // smoothly on two cores
  delay(500);
}
