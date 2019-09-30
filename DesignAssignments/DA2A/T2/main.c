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
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;
    DDRB |= (1 << 3);
	DDRB |= (1 << 2);
	DDRC &= ~(1 << 3);
	
    while (1) 
    {
		PORTB &= ~(1 << 3);
		_delay_ms(2500);
		PORTB |= (1 << 3);
		_delay_ms(3750);
		
		if (~PINC & (1 << 3))
		{
			PORTB &= ~(1 << 2);
			_delay_ms(13330);
			PORTB |= (1 << 2);
		}
    }
}