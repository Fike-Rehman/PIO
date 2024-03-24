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

CRGB ledSolidColor = CRGB::Green; // default color, LED's are off
const int NUM_LEDS = 23;          // number of LEDs in the Ariel strip
CRGB ariel_LEDs[NUM_LEDS] = {0};

enum LEDPattern
{
    STRIP_OFF,
    STRIP_SOLID_COLOR,
    STRIP_KNIGHT_RIDER
};

// initialize Ariel LED pattern
LEDPattern arielLEDPattern = STRIP_OFF; // LED strip off

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