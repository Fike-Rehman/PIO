/*********
  Fike Rehman

  Oberon:
   - Runs a http server on a SONOFF board that listens
   - for incoming requests to trun On/off the on board relay
   - Also listens and responds to the Ping request
   - Can also use on board button to control the relay  
*********/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;


const char* ssid = "OCEANLAB";
const char* password = "FikeNETBB1972";

ESP8266WebServer server(80);

String webPage = "";
String response;

int gpio12Relay = 12;
int gpio13PwrLed = 13;
int gpio0Switch = 0;

// current relay state:
boolean powerOn = false; 

int buttonReading = 0;
int lastButtonReading = 0;

// toggles the current state of the relay
void toggleRelay()
{
  if(powerOn)
  {
    // turn the relay off
    digitalWrite(gpio12Relay, LOW);
    digitalWrite(gpio13PwrLed, HIGH); // LED OFF
    powerOn = false;  
  }
  else
  {
    // turn the relay on
    digitalWrite(gpio12Relay, HIGH);
    digitalWrite(gpio13PwrLed, LOW); // LED OFF
    powerOn = true;  
  }   
}

// Monitors button press. If a button press is detected,
// toggles that a state of the relay.
void MonitorButtonPress()
{
   buttonReading = digitalRead(gpio0Switch);

   if(buttonReading != lastButtonReading)
   {
      if(buttonReading == LOW)
      {
        Serial.println("detected button press...");
        toggleRelay();
      }
      else
      {
        Serial.println("button not pressed");
      }
      
      delay(50);
   }

   lastButtonReading = buttonReading;
}


void setup(void)
{
  webPage += "<h1>WELCOME TO OBERON</h1>";  
  
  Serial.begin(115200); 
  
  // prepare GPIOs
  pinMode(gpio13PwrLed, OUTPUT); // Pwr LED
  pinMode(gpio12Relay, OUTPUT);  // on board Relay
  pinMode(gpio0Switch, INPUT_PULLUP);   // On Board Switch

  // set the Relay to on when starting
  digitalWrite(gpio12Relay, HIGH);
  digitalWrite(gpio13PwrLed, LOW); // LED ON
  powerOn = true;

  delay(5000);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  // Status API: (reports current relay status w/o making any change to the relay)
  server.on("/status", [](){

    response = webPage; 
    response += "Relay Status:";
    response += (powerOn)?"ON" : "OFF";
    server.send(200, "text/html", response);
    
    delay(1000);
    });
  
  // Power on API: (turns the output power On)
  server.on("/on", [](){
    
    digitalWrite(gpio12Relay, HIGH);
    digitalWrite(gpio13PwrLed, LOW);
    powerOn = true;

    response = webPage; 
    response += "Relay Status:";
    response += (powerOn)?"ON" : "OFF";
    server.send(200, "text/html", response);
    
    delay(1000);
  });

  // Power off API (turns the output power off)
  server.on("/off", [](){
    
    digitalWrite(gpio12Relay, LOW);
    digitalWrite(gpio13PwrLed, HIGH); // LED OFF
    powerOn = false;

    response = webPage; 
    response += "Relay Status:";
    response += (powerOn)?"ON" : "OFF";
    server.send(200, "text/html", response);
    
    delay(1000); 
  });

  // Ping API (send the ping single to the board)
  server.on("/ping", [](){
    
    response = webPage; 
    response += "Ping: ACK";
    server.send(200, "text/html", response);
    
    delay(1000); 
  });

  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void)
{
  MonitorButtonPress();
  server.handleClient();
} 



