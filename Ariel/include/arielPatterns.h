/************************************************
 Fike Rehman
 3/24/2024
 arielPatterns.h

 Contains code for all the patterns that can be run on WS2812b LED strip for the project Ariel

 History:
    3/24/2024: Initial code

*********************************/

#include <FastLED.h>
#include <FastLEDHelper.h>

const int NUM_LEDS = 23; // number of LEDs in the Ariel strip

CRGB ariel_LEDs[NUM_LEDS] = {0}; // initialize the LED array

enum LEDPattern
{
    STRIP_OFF,
    STRIP_SOLID_COLOR,
    STRIP_KNIGHT_RIDER
};

CRGB ledSolidColor = CRGB::Green; // default color, LED's are off

// The following is declared as volatile b/c they are modified and accessed by two different task
// initialize Ariel LED pattern
volatile LEDPattern arielLEDPattern = STRIP_OFF; // LED strip off

// following are added for shooting star animation
unsigned long previousMillis = 0;           // Stores last time LEDs were updated
int count = 0;      

void setSolidColor(CRGB color)
{
    fill_solid(ariel_LEDs, NUM_LEDS, color); // fill the LED's with the color green
    FastLED.show();                          // show the LED's
}

void knightRider(CRGB color)
{
    setSolidColor(CRGB::Black);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        ariel_LEDs[i] = color;
        FastLED.show();
        vTaskDelay(50);
    }

    setSolidColor(CRGB::Black);
    // decrements down from end of lights
    for (int i = NUM_LEDS - 1; i >= 0; i--)
    {
        ariel_LEDs[i] = color;
        FastLED.show();
        vTaskDelay(50);
    }

    Serial.println("Knight Rider cycle complete");
}

void shootingStar(int red, int green, int blue, int tail_length, int delay_duration, int interval, int direction){
  /*
   * red - 0 to 255 red color value
   * green - 0 to 255 green color value
   * blue - 0 to 255 blue color value
   * tail_length - value which represents number of pixels used in the tail following the shooting star
   * delay_duration - value to set animation speed. Higher value results in slower animation speed.
   * interval - time between each shooting star (in miliseconds)
   * direction - value which changes the way that the pixels travel (uses -1 for reverse, any other number for forward)
  */
  unsigned long currentMillis = millis();   // Get the time
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;         // Save the last time the LEDs were updated
    count = 0;                              // Reset the count to 0 after each interval
  }
  if (direction == -1) {        // Reverse direction option for LEDs
    if (count < NUM_LEDS) {
      ariel_LEDs[NUM_LEDS - (count % (NUM_LEDS+1))].setRGB(red, green, blue);    // Set LEDs with the color value
      count++;
    }
  }
  else {
    if (count < NUM_LEDS) {     // Forward direction option for LEDs
      ariel_LEDs[count % (NUM_LEDS+1)].setRGB(red, green, blue);    // Set LEDs with the color value
      count++;
    }
  }
  fadeToBlackBy(ariel_LEDs, NUM_LEDS, tail_length);                 // Fade the tail LEDs to black
  FastLED.show();
  delay(delay_duration);                                      // Delay to set the speed of the animation
}