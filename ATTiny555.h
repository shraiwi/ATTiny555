#ifndef _ATTINY555_H_
#define _ATTINY555_H_

#ifndef AT555_TRIGVAL // if it wasnt defined by the user,
#define AT555_TRIGVAL (85) // reset the flip-flop at 85/255 vcc, which is 1/3 vcc
#endif

// initializes the ATTiny555 functionality.
void AT555_begin();

#ifdef AT555_ORIGINAL // ATTiny555 Original Layout
#pragma message "Compiling for original layout..."

/*
 *  Pin # | 555 Pin | Type | Function
 * -------+---------+------+-------------------
 *  PB0   | CTRL    | AIN  | analog ref (ignore)
 *  PB1   | THRESH  | AIN  | on above ctrl, start discharge
 *  PB2   | DIS     | OUT  | cap discharge to gnd, otherwise hi-z
 *  PB3   | TRIG    | AIN  | on below AT555_TRIGVAL/255 vcc, start charge
 *  PB4   | OUT     | OUT  | low during discharge
 */

#ifdef AT555_THRESHVAL
#error "AT555_THRESHVAL is not supported when using the original layout, as it relies on an external voltage divider to supply the TRIG voltage."
#endif

// The value of the ATTiny555 output pin 
#define AT555_outPin (PORTB & (1 << PB4))

void AT555_begin() {
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

ISR(ANA_COMP_vect) {    // on the falling edge (meaning THRESH is now higher than CTRL)
    // reset the flip-flop
    DDRB |= (1 << DDB2);  // enable discharge (set DIS to gnd)
    PORTB |= (1 << PB4);  // enable OUT
}

ISR(ADC_vect) { // on the adc read complete
    ADCSRA |= (1 << ADSC);  // restart ADC measurement

    if (ADCH < AT555_TRIGVAL) {        // if TRIG is less than AT555_TRIGVAL
        // set the flip-flop
        DDRB &= ~(1 << DDB2); // disable discharge (set DIS to hi-z)
        PORTB &= ~(1 << PB4); // disable OUT
    }
}

#else // ATTiny555 Flip Chip Layout
#pragma message "Compiling for flip-chip layout..."

/*
 *  Pin # | 555 Pin | Type | Function
 * -------+---------+------+-------------------
 *  PB1   | DIS     | OUT  | cap discharge to gnd, otherwise hi-z
 *  PB2   | THRESH  | AIN  | on above AT555_THRESHVAL/255, start discharge
 *  PB3   | OUT     | OUT  | low during discharge
 *  PB4   | TRIG    | AIN  | on below AT555_TRIGVAL/255 vcc, start charge
 */

#ifndef AT555_THRESHVAL // if it wasnt defined by the user,
#define AT555_THRESHVAL (170) // set the flip-flop at 170/255 vcc, which is 2/3 vcc
#endif

// The value of the ATTiny555 output pin 
#define AT555_outPin (PORTB & (1 << PB3))

void AT555_begin() {
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
    // because the MUX LSB was 0 when the ADC was measuring PB2/ADC1/THRES,
    // ADCH contains the voltage on THRES.
    // otherwise, ADCH must contain the voltage on TRIG,
    // because the MUX switches between ADC1 and ADC2 each measurement.
    if (ADMUX & (1 << MUX0)) {
        if (ADCH > AT555_THRESHVAL) { // if THRESH is greater than AT555_THRESHVAL,
        // reset the flip-flop
        DDRB |= (1 << DDB1);  // enable discharge (set DIS to gnd)
        PORTB |= (1 << PB3);  // enable OUT
        }
    } else {
        if (ADCH < AT555_TRIGVAL) {        // if TRIG is less than AT555_TRIGVAL
        // set the flip-flop
        DDRB &= ~(1 << DDB1); // disable discharge (set DIS to hi-z)
        PORTB &= ~(1 << PB3); // disable OUT
        }
    }
}

#endif

#endif 

//\\(o-o)//\\ <- oh no, it's a bug in my code!
