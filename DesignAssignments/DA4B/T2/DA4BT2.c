/*
 * DA4BT2.c
 *
 * Created: 12/4/2019 10:59:47 AM
 * Author : Billy
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

// ADC conversion complete vector
// update CTC COMPA on each conversion
ISR(ADC_vect)
{
  unsigned int voltage = ADCH;
  // TIMER2 COMPB = X
  // uses top 4 bits of ADC to select value from 16 to 32
  OCR2B = (voltage >> 4) + 16;
}


// configure TIMER2 for PWM mode with with 50 Hz frequency. prescalar 1024, COMP2A 156
void PWM_init()
{
  // set PORTD3 to output for PWM
  DDRD = (1 << DDRD3);
	
  OCR2A = 156;
		
  TCCR2B |= (1 << WGM22) | (1 << CS22) | (1 << CS21) | (1 << CS20);
  TCCR2A |= (1 << COM2A1) | (0 << COM2A0) | (1 << COM2B1) | (0 << COM2B0) | (1 << WGM21) | (1 << WGM20);
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
    // ADC5 (PC5 PIN28)
    (0 << MUX3) |
    (1 << MUX2) |
    (0 << MUX1) |
    (1 << MUX0);
	
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
	
  // enable those interrupts
  sei();
	
  // initialize PWM TIMER2
  PWM_init();
		
  // the usual infinite loop
  while (1);
}
