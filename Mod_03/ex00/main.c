// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>
// for sei() and ISR
#include <avr/interrupt.h>

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init(void);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

int main(void)
{
	// configurate the LED RGB D in output
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);

	while (1)
	{
		// turn on Red Led then wait a second
		PORTD = (1 << PD5);
		_delay_ms(1000);

		// turn on Green Led then wait a second
		PORTD = (1 << PD6);
		_delay_ms(1000);

		// turn on Blue Led then wait a second
		PORTD = (1 << PD3);
		_delay_ms(1000);
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

void init(void)
{
	
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //

