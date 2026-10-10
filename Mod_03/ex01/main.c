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
	// Configurate the LED RGB as output
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);

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
/*

DDRX -> Data Direction Register -> to defined the direction of a pin:
	DDRD = pins of PORT D
		- Bitn = 1 -> OUTPUT -> the pin send a signal from the microcontroller chip to an outside component 
			-> Exemple: the chip send a signal to the led
		- Bitn = 0 -> INPUT (DEFAULT VALUE) -> pin get a signal from an outside components to the microcontroller chip 
			-> Exemple: a button send an info to the chip

PORTX -> register to define the state of a pin (in output) : page 58 datasheet 
	PORTD -> pins in output of PORT D
		- Bitn = 1 -> HIGH -> the pin give a voltage (close to 5V or 3.3V)
		- Bitn = 0 -> LOW (DEFAULT VALUE) -> the pin don't get voltage (GND/ 0V)
	- Bit PD3(3) -> BLUE
	- Bit PD3(5) -> RED
	- Bit PD3(6) -> GREEN

*/