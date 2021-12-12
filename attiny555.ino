/*
 *  Pin # | 555 Pin | Type | Function
 * -------+---------+------+-------------------
 *  0     | CTRL    | AIN  | analog ref (ignore)
 *  1     | THRESH  | AIN  | on above ctrl, start discharge
 *  2     | DIS     | OUT  | cap discharge to gnd, otherwise hi-z
 *  3     | TRIG    | AIN  | on below 1/3 vcc, start charge
 *  4     | OUT     | OUT  | low during discharge
 */

void setup() {

  cli();            // disable interrupts
  
  // == init adc ==
  ADMUX = 
    (1 << ADLAR) |  // enable 8-bit mode
    (0 << REFS1) |  // set vref to vcc
    (0 << REFS0) |
    (0 << MUX3)  |  // set ADC mux input to TRIG ADC3/PB3
    (0 << MUX2)  |
    (1 << MUX1)  |
    (1 << MUX0);
  ADCSRA = 
    (1 << ADEN) |   // enable the adc
    (1 << ADSC) |   // start conversion immediately
    (1 << ADIE) |   // enable the adc conversion interrupt
    (0 << ADPS2) |  // set prescaler to 2 (500kHz input clock @ 1MHz)
    (0 << ADPS1) |
    (0 << ADPS0);

  // == init comparator ==
  // uses AIN0 (CTRL) and AIN1 (THRESH) as the + and - inputs respectively.
  ACSR = 
    (1 << ACIE) |   // enable the analog comparator interrupt
    (1 << ACIS1) |  // set interrupt on falling edge
    (1 << ACIS0);
  // ACO = CTRL > THRESH, therefore we should interrupt on the falling edge
  
  sei();              	// enable interrupts

  // == init outputs ==
  DDRB = (1 << DDB4);   // set OUT to output
  PORTB |= (1 << PB0);  // enable CTRL 35 KOhm pull-up
}

void loop() {
  // you should be free to do anything here, frighteningly.
  // as long as you don't use the ADC or analog comparator.
  // can YOUR 555 multiprocess?
  // i didn't think so.
}

ISR(ANA_COMP_vect) {    // on the falling edge (meaning THRESH is now higher than CTRL)
  // reset the flip-flop
  DDRB |= (1 << DDB2);  // enable discharge (set DIS to gnd)
  PORTB |= (1 << PB4);  // enable OUT
}

ISR(ADC_vect) { // on the adc read complete
  ADCSRA |= (1 << ADSC);  // restart ADC measurement

  if (ADCH < 85) {        // if TRIG is less than 1/3 vcc
    // set the flip-flop
    DDRB &= ~(1 << DDB2); // disable discharge (set DIS to hi-z)
    PORTB &= ~(1 << PB4); // disable OUT
  }
}
