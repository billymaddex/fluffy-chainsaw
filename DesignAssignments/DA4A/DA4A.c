/*
 * DA4A.c
 *
 * Created: 11/11/2019 8:45:32 AM
 * Author : Billy
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

// pin change vector
// toggle PWM output signal on each button press
ISR(PCINT0_vect)
{
  if(~PINB & (1 << 7))
    {
      DDRD ^= (1 << DDRB3);
    }
}

// ADC conversion complete vector
// update PWM duty cycle on each conversion
ISR(ADC_vect)
{
  unsigned int voltage = ADCH;
  OCR2B = (voltage * 242) / 255;
}

// configure pin change interrupt for PINB7
void button_init()
{
  PCMSK0 = (1 << PCINT7);
  PCICR = (1 << PCIE0);
}

// configure TIMER2 for non-inverting fast PWM, OCR2B/PIND3 output,prescalar 64
void PWM_init()
{
  TCCR2B |= (0 << WGM22) | (1 << CS22) | (0 << CS21) | (0 << CS20);
  TCCR2A |= (1 << COM2B1) | (0 << COM2B0) | (1 << WGM21) | (1 << WGM20);
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
  // initialize input buttons
  button_init();
	
  // initialize PWM
  PWM_init();
	
  // initialize ADC
  ADC_init();
	
  // enable those interrupts
  sei();
	
  // the usual infinite loop
  while (1);
}
