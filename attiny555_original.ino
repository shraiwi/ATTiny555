// ATTiny555 Original Version!

/*
 *  Pin # | 555 Pin | Type | Function
 * -------+---------+------+-------------------
 *  1     | DIS     | OUT  | cap discharge to gnd, otherwise hi-z
 *  2     | THRESH  | AIN  | on above 2/3 vcc, start discharge
 *  3     | OUT     | OUT  | low during discharge
 *  4     | TRIG    | AIN  | on below 1/3 vcc, start charge
 */

 // ADC2/PB4/TRIG = mux 0001
 // ADC1/PB2/THRESH = mux 0010

void setup() {

  cli();            // disable interrupts
  
  // == init adc ==
  ADMUX = 
    (1 << ADLAR) |  // enable 8-bit mode
    (0 << REFS1) |  // set vref to vcc
    (0 << REFS0) |
    (0 << MUX3)  |  // set ADC MUX input to ADC1/PB2/THRESH
    (0 << MUX2)  |
    (0 << MUX1)  |
    (1 << MUX0);
  ADCSRA = 
    (1 << ADEN) |   // enable the ADC
    (1 << ADSC) |   // start conversion immediately
    (1 << ADIE) |   // enable the adc conversion interrupt
    (0 << ADPS2) |  // set prescaler to 2 (500kHz input clock @ 1MHz)
    (0 << ADPS1) |
    (0 << ADPS0);
  
  sei();              	// enable interrupts

  // == init outputs ==
  DDRB = (1 << DDB3);   // set OUT to output
}

void loop() {
  // you should be free to do anything here, frighteningly.
  // as long as you don't use the ADC or analog comparator.
  // can YOUR 555 multiprocess?
  // i didn't think so.
}

ISR(ADC_vect) { // on the ADC read complete
  ADCSRA |= (1 << ADSC);  // restart ADC measurement

  // we now have about 20 cycles to do whatever we want. gotta do it fast!

  // flip the ADC MUX 2 LSBs.
  // if MUX is set to 0001 (PB4/ADC2/TRIG), 
  // it will now be 0010 (PB2/ADC1/THRES) and vice versa.
  ADMUX ^=
    (1 << MUX1) |
    (1 << MUX0);

  // if the MUX LSB is now 1, it must have been 0 during the last conversion.
  // because the LSB was 0 when the ADC was measuring PB2/ADC1/THRES,
  // ADCH contains the voltage on THRES.
  // otherwise, ADCH must contain the voltage on TRIG,
  // because the MUX switches between ADC1 and ADC2 each measurement.
  if (ADMUX & (1 << MUX0)) {
    if (ADCH > 170) { // if THRESH is greater than 2/3 VCC,
      // reset the flip-flop
      DDRB |= (1 << DDB1);  // enable discharge (set DIS to gnd)
      PORTB |= (1 << PB3);  // enable OUT
    }
  } else {
    if (ADCH < 85) {        // if TRIG is less than 1/3 VCC
      // set the flip-flop
      DDRB &= ~(1 << DDB1); // disable discharge (set DIS to hi-z)
      PORTB &= ~(1 << PB3); // disable OUT
    }
  }
  
  
}
