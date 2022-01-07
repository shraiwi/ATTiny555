#define AT555_DISABLE_OUT // override the output pin, because FastLED needs it.

#include <FastLED.h>
#include "ATTiny555.h"

#define NUM_LEDS (12)

CRGB leds[NUM_LEDS];

void setup() {
  AT555_begin(); 
  FastLED.addLeds<WS2812B, AT555_OUTPIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(16); // saves our eyes
}

void loop() {
  // if the 555 is oscillating,
  if (ADCH >= AT555_TRIGVAL && ADCH <= AT555_THRESHVAL) {

    // get how far the analog voltage is between 1/3 vcc and 2/3 vcc
    uint8_t progress = ADCH - AT555_TRIGVAL; 
    progress = (progress << 2) - progress; // multiply by 3

    // find how many leds we need to light to represent the voltage
    uint8_t ledsLit = ((uint16_t)progress * NUM_LEDS) >> 8;

    // get how far the progress is between two leds (from 0-255)
    uint8_t ledProgress = (uint8_t)((uint16_t)progress - ((uint16_t)ledsLit << 8) / NUM_LEDS) * NUM_LEDS;
  
    CRGB col = AT555_flipFlop ? CRGB::Green : CRGB::Red; // red when OUT pin is low, green when OUT pin is high

    // fill the led colors according to the graph
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      
      if (i < ledsLit) leds[i] = col
      else if (i == ledsLit) { // if the LED is the boundary LED,
        leds[i] = col.scale8(ledProgress); // scale the colors to anti-alias the line bar.
      }
      else leds[i] = CRGB::Black;
    }

    FastLED.show(); // display the changes
  }
}
