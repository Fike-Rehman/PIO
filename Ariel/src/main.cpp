#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char *ssid = "OCEANLAB";
const char *password = "FikeNETBB1972";

const int LEDPIN = 2;
const int PUSH_BUTTON = 15;

int buttonReading = 0;
int lastButtonReading = 0;

boolean ledOn = false;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup()
{
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("The IP address assigned to ESP32 is: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  // set time offset in seconds to adjust for your timezone, Central Standard Time is UTC -5 hours
  timeClient.setTimeOffset(-18000);

  pinMode(LEDPIN, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLDOWN);

  // set the LED to off when starting
  digitalWrite(LEDPIN, LOW);
  int ledOn = false;
}

void toggleLED()
{

  if (ledOn)
  {
    digitalWrite(LEDPIN, LOW);
    ledOn = false;
  }
  else
  {
    digitalWrite(LEDPIN, HIGH);
    ledOn = true;
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
      toggleLED();
    }
    delay(50);
  }
  lastButtonReading = buttonReading;
}

void loop()
{
  timeClient.update();
  MonitorButtonPress();

  // print current time from timeClient
  // Serial.println("Curent Time in Minneapolis is:" + timeClient.getFormattedTime());
  // delay(1000);
}
