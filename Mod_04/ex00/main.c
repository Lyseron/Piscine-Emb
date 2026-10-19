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

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init(void);

void INT0_vect(void) __attribute__((signal));

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

int main(void)
{
	init();

	// Enable global interupt (sei())
	__asm__ __volatile__ ("sei" ::: "memory");

	while (1)
	{}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Called when a falling edge is detected on PD2 (button pressed)
void INT0_vect(void)
{
	// Toggle the Led
	PORTB ^= (1 << LED);
	_delay_ms(30);

	// Clear pending interupt
	EIFR = (1 << INTF0);
}

void init(void)
{
	// Set OUTPUT for the Led and button
	DDRB |= (1 << LED);
	DDRD &= ~(1 << BUTTON);

	// For the internal pull up
	PORTD |= (1 << BUTTON);

	// ----------- Interuption ------------- //
	// Descending front for the button generates an interupt request (when press -> interuption)
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);

	// Clear pending interupt before enabling it
	EIFR = (1 << INTF0);

	// Enable the external interupt for the button (INT0)
	EIMSK |= (1 << INT0);

}

// ------------------------------------------- NOTES ---------------------------------------------------------- //

// ---------- REGISTER & BIT ----------- //
/*
DDRX -> Data Direction Register -> to defined the direction of a pin:
	DDRD = pins of PORT D
		- Bitn = 1 -> OUTPUT -> the pin send a signal from the microcontroller chip to an outside component 
			-> Exemple: the chip send a signal to the led
		- Bitn = 0 -> INPUT (DEFAULT VALUE) -> pin get a signal from an outside components to the microcontroller chip 
			-> Exemple: a button send an info to the chip
			Datasheet -> page 72 - 73

PORTX -> register to define the state of a pin (in output) : page 58 datasheet 
	PORTD -> pins in output of PORT D
		- Bitn = 1 -> HIGH -> the pin give a voltage (close to 5V or 3.3V)
		- Bitn = 0 -> LOW (DEFAULT VALUE) -> the pin don't get voltage (GND/ 0V)
	- Bit PD3(3) -> BLUE
	- Bit PD3(5) -> RED
	- Bit PD3(6) -> GREEN
	Datasheet -> page 59 to 73

// ---------- INTERUPT ----------------- //

EICRA -> External Interrupt Control Register Canal A
	-> Defines what event triggers an externals interrupt
	->Bit ICS01(1) = 1, ISC00 = 0 -> INT0 is triggered on the falling edge (when button is pressed)
	Datasheet page -> 54

EIMSK -> External Interrupt Mask Register
	-> Enables or disable individual external interrupts
	-> INT0 (bit 0)
		-> Bit = 1 -> INT0 enabled
		-> Bit = 0 -> INT0 disabled

EIFR -> External interupt Flag register
	-> Store pending external interupt requests
	-> When a flag bit is set to 1, it means that an external interrupt request has occured
	-> We clear the flag to prevent a pending interrupt from being handle when we re-enable the interupt
	-> To clear a flag bit, we must write a 1 to it.
	Datasheet page -> 55

SREG -> AVR Status Register
	-> I bit(7) -> Global Interuption Enable
	sei() set the I bit to 1
	Datasheet page -> 11 - 16

void INT0_vect(void) __attribute__((signal))
	-> ISR function but defined directly
	-> void INT0_vect -> Interuption for INT0 when changes (here button)
	-> __attribute((signal)) -> tell the compiler that it is not a regular c fonction but an interuption fonction
	->Function ISR interupt -> https://github.com/avrdudes/avr-libc/blob/main/include/avr/interrupt.h

*/