// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>
// for sei() and ISR
#include <avr/interrupt.h>
// to use boolean
#include <stdbool.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define BUTTON PD1
#define LED PB0

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init(void);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

int main(void)
{
	init();
	sei();

	while (1)
	{
		_delay_ms(10);
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

void init(void)
{
	// Set OUTPUT for the Led and button
	DDRB |= (1 << LED) | (1 << BUTTON);

	// Configurate the external interput for the button
	EIMSK |= (1 << INT0);

	// descending front for the button generates an interupt request (when press (LOW) -> interuption)
	EICRA |= (1 << ISC01);

	// Toggle the led
	PORTB ^= (1 << LED);
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

EICRA -> External Interrupt Control Register Canal A
	->Bit ICS01(1)
	Datasheet page -> 54

*/