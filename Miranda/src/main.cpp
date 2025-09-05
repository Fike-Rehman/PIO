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

#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}