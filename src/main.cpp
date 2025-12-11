#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t delayCounter = 0;

ISR(TIMER0_COMPA_vect){
  delayCounter++;
}

uint8_t state3 = 0; // state of PB3
uint8_t state4 = 0; // state of PB4

void setup() {

  DDRB = (1<<PB3) | (1<<PB4); // set pins as outputs
  
  TIMSK = bit(OCIE0A);  // enable timer0 interrupt
  SREG |= bit(SREG_I);  // enable global interrupts
  
  /*
  timer0 setup:
    - CLK/1024 prescale
    - 102 mS pulse width
    - TODO: attach interrupt to get to ~1 S
  */
  {
    TCCR0A = bit(WGM01);  // set mode to CTC
    OCR0A = (uint8_t)100; // 8MHz * TCCR0B * (T_uS/2)
    TCCR0B = bit(CS02) | bit(CS00);  // clk/1024
  }

  /*
  timer1 setup:
    - CLK/8 prescale
    - 32 MHz synchronious clock i think
    - 59 uS pulse width = 8 kHz data out
  */
  {
    OCR1A = 200; // 32MHz * TCCR0B * (T_uS/2)
    TCCR1 = bit(CS12);  // clk/8
    PLLCSR = bit(LSM) | bit(PCKE) | bit(PLLE);  // set clock to 32 MHz
  }

}

void loop() {

  if(delayCounter >= 11){  // should be ~1.1 second
    // PINB = bit(PB3);  // toggle PB3
    switch (state3)
    {
    case 0:
      state3 = 1;
      PORTB |= bit(PB3);
      break;
    
    case 1:
      state3 = 0;
      PORTB &= ~bit(PB3);
      break;

    default:
      state3 = 0;
      break;
    }
    TCNT0 = 0;  // reset the timer
    delayCounter = 0;
  }

  if(TIFR & bit(OCF1A)){  // if timer1 has matched OCR0A
    // PINB = bit(PB4);  // toggle PB4
    switch (state4)
    {
    case 0:
      state4 = 1;
      PORTB |= bit(PB4);
      break;
    
    case 1:
      state4 = 0;
      PORTB &= ~bit(PB4);
      break;
      
    default:
      state4 = 0;
      break;
    }
    TCNT1 = 0;  // reset the timer
    TIFR = bit(OCF1A);  // reset the flag
  }
}
