#include <FastLED.h>

CRGB getColorFromString(const String &colorStr)
{
    if (colorStr.equalsIgnoreCase("red"))
    {
        return CRGB::Red;
    }
    else if (colorStr.equalsIgnoreCase("green"))
    {
        return CRGB::Green;
    }
    else if (colorStr.equalsIgnoreCase("blue"))
    {
        return CRGB::Blue;
    }
    else if (colorStr.equalsIgnoreCase("magenta"))
    {
        return CRGB::Magenta;
    }
    else if (colorStr.equalsIgnoreCase("cyan"))
    {
        return CRGB::Cyan;
    }
    else if (colorStr.equalsIgnoreCase("red"))
    {
        return CRGB::Red;
    }
    else if (colorStr.equalsIgnoreCase("purple"))
    {
        return CRGB::Purple;
    }
    else if (colorStr.equalsIgnoreCase("lime"))
    {
        return CRGB::Lime;
    }
    else if (colorStr.equalsIgnoreCase("teal"))
    {
        return CRGB::Teal;
    }
    else if (colorStr.equalsIgnoreCase("indigo"))
    {
        return CRGB::Indigo;
    }
    else if (colorStr.equalsIgnoreCase("chartreuse"))
    {
        return CRGB::Chartreuse;
    }
    else if (colorStr.equalsIgnoreCase("silver"))
    {
        return CRGB::Silver;
    }
    else if (colorStr.equalsIgnoreCase("slateGray"))
    {
        return CRGB::SlateGray;
    }
    else if (colorStr.equalsIgnoreCase("darkSlateGray"))
    {
        return CRGB::DarkSlateGray;
    }
    else if (colorStr.equalsIgnoreCase("seaGreen"))
    {
        return CRGB::SeaGreen;
    }
    else if (colorStr.equalsIgnoreCase("darkGreen"))
    {
        return CRGB::DarkGreen;
    }
    else if (colorStr.equalsIgnoreCase("skyBlue"))
    {
        return CRGB::SkyBlue;
    }
    else if (colorStr.equalsIgnoreCase("deepSkyBlue"))
    {
        return CRGB::DeepSkyBlue;
    }
    else if (colorStr.equalsIgnoreCase("aquamarine"))
    {
        return CRGB::Aquamarine;
    }
    else if (colorStr.equalsIgnoreCase("powderBlue"))
    {
        return CRGB::PowderBlue;
    }
    else if (colorStr.equalsIgnoreCase("blueViolet"))
    {
        return CRGB::BlueViolet;
    }
    else if (colorStr.equalsIgnoreCase("darkViolet"))
    {
        return CRGB::DarkViolet;
    }
    else if (colorStr.equalsIgnoreCase("mediumVioletRed"))
    {
        return CRGB::MediumVioletRed;
    }
    else if (colorStr.equalsIgnoreCase("orchid"))
    {
        return CRGB::Orchid;
    }
    else
    {
        return CRGB::Black; // Default color if not recognized
    }
}