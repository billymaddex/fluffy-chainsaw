/*
 * DA2AT2.c
 *
 * Created: 9/29/2019 7:42:05 PM
 * Author : Billy
 */ 

#include <avr/io.h>
#include <util/delay.h>


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

      // if swith is pressed, flash the other light
      if (~PINC & (1 << 3))
	{
	  // turn the light on
	  PORTB &= ~(1 << 2);
	  // call delay
	  _delay_ms(13330);
	  // turn the light off
	  PORTB |= (1 << 2);
	}
    }
}
