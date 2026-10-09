// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define RED PD5
#define GREEN PD6
#define BLUE PD3

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

int main(void)
{
	// configurate the LED RGB D in output
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);

	while (1)
	{
		// turn on Red Led then wait a second
		PORTD = (1 << RED);
		_delay_ms(1000);

		// turn on Green Led then wait a second
		PORTD = (1 << GREEN);
		_delay_ms(1000);

		// turn on Blue Led then wait a second
		PORTD = (1 << BLUE);
		_delay_ms(1000);
	}
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*

DDRD -> 

PORTD ->
	- Bit PD3(3) ->
	- Bit PD5(5) ->
	- Bit PD6(6) ->

*/