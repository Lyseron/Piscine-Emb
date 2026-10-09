// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>
// for sei() and ISR
#include <avr/interrupt.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define RED PD5
#define GREEN PD6
#define BLUE PD3

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

int main(void)
{
	// Configurate the LED RGB D in output
	DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6);

	while (1)
	{
		// Turn on Red Led then wait a second
		PORTD = (1 << RED);
		_delay_ms(1000);

		// Turn on Green Led then wait a second
		PORTD = (1 << GREEN);
		_delay_ms(1000);

		// Turn on Blue Led then wait a second
		PORTD = (1 << BLUE);
		_delay_ms(1000);

		// Turn on yellow (Red + Green)
		PORTD = (1 << RED) | (1 << GREEN);
		_delay_ms(1000);

		// Turn on cyan (Green + Blue)
		PORTD = (1 << GREEN) | (1 << BLUE);
		_delay_ms(1000);

		// Turn on magenta (Red and Blue)
		PORTD = (1 << RED) | (1 << BLUE);
		_delay_ms(1000);

		// Turn on white (Red + Green + Blue)
		PORTD = (1 << RED) | (1 << GREEN) | (1 << BLUE);
		_delay_ms(1000);
	}
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //

