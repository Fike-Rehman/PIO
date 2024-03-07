/*
 * CallistoDHT22: Hosts a web service that responds to incoming requests regarding:
 *
 *    1) Last Known Temperature, reading from the DHT22 sensor on Pin 15
 *    2) Last Known Humidity reading from DHT22 sensor on Pin 15
 *    3) A calculated value of HeatIndex based on last known Temp/Humidty values
 *    4) A Ping request (with a 'ACK' response)
 *
 *    The program reads the latest temp/humdity data from the data pin of the DHT22 sensor (Pin 15)
 *    every 6 minutes (10 readings/hr) and updates the program memory. When a request arrives, the
 *    values for the latest temperature and Hummidity readings are returned as repsonse. A calculated
 *    value for the heat index based on the latest temp/humidity data is also available.
 *
 *    While reading the DHT22 data pin (Pin 15), the green LED is turn on for one second
 *    While responding to a ping request, blue LED is turned on for one second
 *
 *   Fike Rehman
 *   November 25, 2019
 */

#include <Arduino.h>
#include <config.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <DHT.h>

// Pin Definitions
const int wifiErrorPin = 5;   // Thing's onboard LED
const int dhtSensorPin = 4;   // Dht Sensor read pin
const int dataReadLEDPin = 0; // Data read LED (green) pin
const int pingLEDPin = 14;    // ping LED (blue) Pin

// State Variables:
float tempF;      // temp. reading from the DHT sensor in Fahrenheit (set to 1000 if read fails)
float tempC;      // temp. reading from the DHT sensor in Celsius (set to 1000 if read fails)
float humidity;   // Humidity reading from DHT sensor (percent)  (set to 1000 if read fails)
float heatIndexF; // computed value of heat Index in Fahrenheit
float heatIndexC; // computed value of heat Index in Centigrade

const long interval = 60 * 1000; // interval b/w readings (6 minutes)

unsigned long previousMillis = 0; // stores last time sensor reading was taken

// Initialize DHT sensor.
DHT dht(dhtSensorPin, DHT22);

WiFiServer server(80);

void UpdateDHTReadings()
{
  digitalWrite(dataReadLEDPin, HIGH);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();

  // Read temperature as Celsius (the default)
  tempC = dht.readTemperature();

  // Read temperature as Fahrenheit (isFahrenheit = true)
  tempF = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(tempC) || isnan(tempF))
  {
    Serial.println("Failed to read from DHT sensor!");
    digitalWrite(dataReadLEDPin, LOW);
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  heatIndexF = dht.computeHeatIndex(tempF, humidity);

  // Compute heat index in Celsius (isFahreheit = false)
  heatIndexC = dht.computeHeatIndex(tempC, humidity, false);

  digitalWrite(dataReadLEDPin, LOW);
}

void blinkPingLED()
{
  digitalWrite(pingLEDPin, HIGH);
  delay(3000);
  digitalWrite(pingLEDPin, LOW);
}

void connectWiFi()
{
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(wifiErrorPin, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMDNS()
{
  // Call MDNS.begin(<domain>) to set up mDNS to point to
  // "<domain>.local"
  if (!MDNS.begin("thing"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
}

void setup()
{
  Serial.begin(9600);

  dht.begin();

  pinMode(wifiErrorPin, OUTPUT);
  digitalWrite(wifiErrorPin, HIGH);

  pinMode(dataReadLEDPin, OUTPUT);
  digitalWrite(dataReadLEDPin, LOW);

  pinMode(pingLEDPin, OUTPUT);
  digitalWrite(pingLEDPin, LOW);

  connectWiFi();
  server.begin();
  setupMDNS();

  // get the first sensor readings:
  UpdateDHTReadings();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    UpdateDHTReadings();
  }
  // Here we respond to incoming requests:
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println("incoming request:");
  Serial.println(request);
  client.flush();

  if (request.indexOf("/TemperatureF") != -1)
  {
    // client is looking for current Temp in Fahrenheit:
    s += "Temperature(F): " + String(tempF, 2) + " degrees";
  }
  else if (request.indexOf("/TemperatureC") != -1)
  {
    // client is looking for current Temp in Celsius
    s += "Temperature(C): " + String(tempC, 2) + " degrees";
  }
  else if (request.indexOf("/Humidity") != -1)
  {
    // client is looking for current Humidity
    s += "Humidity: " + String(humidity, 2) + " percent ";
  }
  else if (request.indexOf("/HeatIndexF") != -1)
  {
    // client is looking for current Heat index in F
    s += "Heat Index(F): " + String(heatIndexF, 2) + " degrees";
  }
  else if (request.indexOf("/HeatIndexC") != -1)
  {
    // client is looking for current Heat index in C
    s += "Heat Index(C): " + String(heatIndexC, 2) + " degrees";
  }
  else if (request.indexOf("/Ping") != -1)
  {
    // first blink the LED:
    blinkPingLED();

    // client is looking for Ping confirmation
    s += "Ping: ";
    s += "ACK";
  }
  else
  {
    // request is invalid
    s += "Invalid Request.<br> Try /WindowOpen, or /AirflowDetected or/AlarmStatus or /Ping";
  }

  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}
