//#define AT555_ORIGINAL // comment out to use the original layout

/*
 * you can define the trigger and threshold voltages if you want,
 * expressed as the numerator of a fraction whose denominator is 255.
 * 
 * by default, the TRIG and THRESH voltages emulate a 555 at 1/3 and 2/3 of vcc, respectively.
 */
//#define AT555_TRIGVAL   (??)
//#define AT555_THRESHVAL (??) // not supported in the original layout!

#include "ATTiny555.h"

void setup() {
  AT555_begin();
}

void loop() {
  // you should be free to do anything here, frighteningly.   
  // as long as you don't use the ADC or analog comparator.
  // can YOUR 555 multiprocess?
  // i didn't think so.

  // you can also read the OUTPUT pin value using the AT555_outPin macro.
}
