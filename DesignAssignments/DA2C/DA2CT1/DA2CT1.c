/*
 * DA2CT1.c
 *
 * Created: 10/12/2019 9:46:07 AM
 * Author : Billy
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

// timer based delay function, clock 0
int clock0_ms (int ms)
{
	// set clock 1 to normal operation
	TCCR0A = 0x00;
	// 250 ticks
	TCNT0 = 0x06;
	// start clock 1 with prescalar = 64
	TCCR0B |= (1 << CS01) | (1 << CS00);
	
	// run the clock for 0.001s ms times
	for (int i = ms; i > 0; i--)
	{
		// poll the clock
		while ((TIFR0 & 0x01) == 0);
		// 250 ticks
		TCNT0 = 0x06;
		// reset overflow flag
		TIFR0 = 0x01;
	}
	
	// stop clock 1
	TCCR0B &= ~(1 << CS01) & ~(1 << CS00);
	
	return 0;
}

// timer based delay function, clock 0
int clock2_ms (int ms)
{
	// set clock 2 to normal operation
	TCCR2A = 0x00;
	// 250 ticks
	TCNT2 = 0x06;
	// start clock 2 with prescalar = 64
	TCCR2B |= (1 << CS22);
	
	// run the clock for 0.001s ms times
	for (int i = ms; i > 0; i--)
	{
		// poll the clock
		while ((TIFR2 & 0x01) == 0);
		// 250 ticks
		TCNT2 = 0x06;
		// reset overflow flag
		TIFR2 = 0x01;
	}
	
	// stop clock 2
	TCCR2B &= ~(1 << CS22);
	
	return 0;
}

// long flash on secondary LED
int light2 (void)
{
	// turn the light on
	PORTB &= ~(1 << 2);
	// call delay
	clock2_ms(1333);
	// turn the light off
	PORTB |= (1 << 2);
	
	return 0;
}

// external interrupt handler
ISR (INT0_vect)
{
	if (~PIND & (1 << 2)) light2();
}

// pin change interrupt handler
ISR (PCINT1_vect)
{
	if (~PINC & (1 << 3)) light2();
}

int main(void)
{
  // Set DDR B,C,D to known initial value
  DDRB = 0x00;
  DDRC = 0x00;
  DDRD = 0x00;
  // set PORT B,C,D to know initial value
  PORTB = 0xFF;
  PORTC = 0xFF;
  PORTD = 0xFF;
  // set PORTB 2 and 3 to output
  DDRB |= (1 << 3);
  DDRB |= (1 << 2);
  // set PINC 3 to input
  DDRC &= ~(1 << 3);
  
  // configure interrupt handling
  // enable INT0, falling edge trigger
  EIMSK = (1 << INT0);
  EICRA = 0x02;
  // enable PCINT11 (PINC.3)
  PCMSK1 = 0x08;
  PCICR = (1 << PCIE1);

  // enable global interrupt
  sei();
	
  // LED pulse
  while (1) 
    {
      // turn the light on
      PORTB &= ~(1 << 3);
      // call timer based delay
      clock0_ms(250);
      // turn the light off
      PORTB |= (1 << 3);
      // call delay
      clock0_ms(375);
    }
}
