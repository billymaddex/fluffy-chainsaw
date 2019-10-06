/*
 * DA2BT2.c
 *
 * Created: 10/5/2019 2:28:51 PM
 * Author : Billy
 */ 

#define F_CPU 1600000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// long flash on secondary LED
int light2 (void)
{
	// turn the light on
	PORTB &= ~(1 << 2);
	// call delay
	_delay_ms(13330);
	// turn the light off
	PORTB |= (1 << 2);
	
	return 0;
}

// external interrupt handler
ISR (INT0_vect)
{
	if (~PIND & (1 << 2))
	{
		light2();
	}
}

// pin change interrupt handler
ISR (PCINT1_vect)
{
	if (~PINC & (1 << 3))
	{
		light2();
	}
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
      // call delay
      _delay_ms(2500);
      // turn the light off
      PORTB |= (1 << 3);
      // call delay
      _delay_ms(3750);

      // if switch is pressed, flash the other light
      /*if (~PINC & (1 << 3))
		{
			light2();
		}*/
    }
}
