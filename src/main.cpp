#include <Arduino.h>
#include <avr/io.h>

void setup() {

  DDRB = (1<<PB3) | (1<<PB4); // set pins as outputs
  
  // TIMSK = bit(OCIE0A) | bit(OCIE1A);  // enable interrupts on output compare registers A
  
  /*
  timer0 setup:
    - CLK/1024 prescale
    - 204.6 mS pulse width
    - TODO: attach interrupt to get to ~1 S
  */
  {
    TCCR0A = 0;  // set mode to normal
    OCR0A = (uint8_t)200; // 8MHz * TCCR0B * (T_uS/2)
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
  if(TIFR & bit(OCF0A)){  // if timer0 has matched OCR0A
    PINB = bit(PB3);  // toggle PB3
    TCNT0 = 0;  // reset the timer
    TIFR = bit(OCF0A);  // reset the flag
  }

  if(TIFR & bit(OCF1A)){  // if timer1 has matched OCR0A
    PINB = bit(PB4);  // toggle PB4
    TCNT1 = 0;  // reset the timer
    TIFR = bit(OCF1A);  // reset the flag
  }
}
