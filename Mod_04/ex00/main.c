// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for sei() and ISR
#include <avr/interrupt.h>
// for the _delay_ms
#include <util/delay.h>
#include <stdbool.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define BUTTON PD2
#define LED PB0

volatile bool button_pressed = false;

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init(void);
void INT0_vect(void) __attribute__((signal));

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

int main(void)
{
	init();
	sei();

	while (1)
	{
		if (button_pressed == true)
		{
			// Wait for the button to stabilize
			_delay_ms(20);

			// Wait for the button to be release
			while (!(PIND & (1 << BUTTON)))
			{}
			// Wait for the button to stabilize
			_delay_ms(20);

			// if the button was pressed and now is realease
			if (PIND & (1 << BUTTON))
			{
				// Set the variable to false for the next press of button
				button_pressed = false;

				// Clear waiting interuption
				EIFR = (1 << INTF0);

				// Enable the external interupt again
				EIMSK |= (1 << INT0);
				EIMSK &= ~(1 << INT1);
			}
		}
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

void INT0_vect(void)
{
	// Toggle the led
	PORTB ^= (1 << LED);

	// Disable the external interupt for the button temporarily
	EIMSK &= ~(1 << INT0);

	// Signal that the button was pressed
	button_pressed = true;
}

void init(void)
{
	// Set OUTPUT for the Led and button
	DDRB |= (1 << LED);
	DDRD &= ~(1 << BUTTON);

	// For the internal pull up
	PORTD |= (1 << BUTTON);

	// Descending front for the button generates an interupt request (when press (LOW) -> interuption)
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);

	// Clear waiting interuption
	EIFR = (1 << INTF0);

	// Configurate the external interupt for the button INT0
	EIMSK |= (1 << INT0);
	EIMSK &= ~(1 << INT1);

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

EIFR -> External interupt Flag register
	-> When a flag bit is set to 1, it means that an external interrupt request has occured
	-> We clear the flag to prevent a pending interrupt from being handle when we re-enable the interupt
	-> To clear a flag bit, we must write a 1 to it.
	Datasheet page -> 55

void INT0_vect(void) __attribute__((signal))
	-> ISR function but defined directly
	-> void INT0_vect -> Interuption for INT0 when changes (here button)
	-> __attribute((signal)) -> tell the compiler that it is not a regular c fonction but an interuption fonction
	->Function ISR interupt -> https://github.com/avrdudes/avr-libc/blob/main/include/avr/interrupt.h
*/