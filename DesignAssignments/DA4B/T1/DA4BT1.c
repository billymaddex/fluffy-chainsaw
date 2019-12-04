/*
 * DA4B.c
 *
 * Created: 12/3/2019 11:42:58 PM
 * Author : Billy
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>


// global variable to control motor step
int step = 0;

// ADC conversion complete vector
// update CTC COMPA on each conversion
ISR(ADC_vect)
{
  unsigned int voltage = ADCH;
  // TIMER1 COMPA = X
  OCR1A = (( (float) voltage / 255.0) * 1480) + 80;
}

// TIMER1 CTC interrupt handler
ISR(TIMER1_COMPA_vect)
{
  // check step variable to move through full step sequence
  if (step == 0)
    {
      PORTD = 0x09;
    }
  else if (step == 1)
    {
      PORTD = 0x03;
    }
  else if (step == 2)
    {
      PORTD = 0x06;
    }
  else if (step == 3)
    {
      PORTD = 0x0C;
    }
	
  // increment step
  step++;
	
  // reset step when greater than 3
  if (step > 3)
    {
      step = 0;
    }
}

// configure TIMER1 for CTC mode with interrupt enabled, prescalar 1024
void CTC_init()
{
  // configure interrupt handling
  // enable TIMER1 COMPA interrupt
  TIMSK1 |= (1 << OCIE1A);
	
  // set safe initial value for COMPA
  OCR1A = 1560;
	
  // set TIMER1 to CTC operations
  TCCR1B |= (1 << WGM12);
		
  // start TIMER1 with prescalar = 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
}

// enable ADC on PINC5, free running, interrupt enable
void ADC_init()
{	
  ADMUX =
    // Reference Selection Bits
    // AVcc with external capacitor at AREF
    (0 << REFS1) |
    (1 << REFS0) |
    // ADC Left Adjust Result
    (1 << ADLAR) |
    // Analog Channel Selection Bits
    // ADC0 (PC0)
    (0 << MUX3) |
    (0 << MUX2) |
    (0 << MUX1) |
    (0 << MUX0);
	
  ADCSRA =
    // ADC ENable
    (1 << ADEN) |
    // ADC Start Conversion
    (1 << ADSC) |
    // ADC Auto Trigger Enable
    (1 << ADATE) |
    // ADC Interrupt Flag
    (0 << ADIF) |
    // ADC Interrupt Enable
    (1 << ADIE) |
    // ADC Prescaler Select Bits
    // prescalar = 128
    (1 << ADPS2) |
    (1 << ADPS1) |
    (1 << ADPS0);
	
  ADCSRB = 
    // disable analog comparator
    (0 << ACME) |
    // enable free running mode
    (0 << ADTS2) |
    (0 << ADTS1) |
    (0 << ADTS0);
}

int main(void)
{
  // initialize ADC
  ADC_init();
	
  // initialize TIMER1
  CTC_init();
	
  // enable those interrupts
  sei();
	
  // set PORTD 0,1,2,3 to output
  DDRD = 0x0F;
	
  // the usual infinite loop
  while (1);
}
