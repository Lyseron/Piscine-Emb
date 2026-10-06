// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>

int main(void)
{
//	|= changed just the concern bit to 1, so PB0 in OUTPUT (the microcontroler send a signal to the component)
	DDRB |= (1 << PB0);

// &= ~ changed just the concern bit to 0, so PD2 in INPUT (the chips read from it)
	DDRD &= ~(1 << PD2);

//	|= changed just the concern bit to 1, to activate the internal pullup*
	PORTD |= (1 << PD2);

// to infinitly run the program
	while (1)
	{
	// & to read a bit -> check if the concern bit is HIGH or LOW (HIGH if not null)
		if ((PIND & (1 << PD2)) == 0)
		//	|= changed just the concern bit to 1, so PB0 in HIGH
			PORTB |= (1 << PB0);
		else
		// &= ~ put the concern bit to 0, so PB0 is LOW
			PORTB &= ~(1 << PB0);
	}
}

// PORT B -> groups physics of pin (from PB0 to PB7), each pin correspond to a bit and has it's own role
	// PB0 -> bit 0 in PORT B = LED D1 in schematics

// DDRX = Data Direction Register -> to defined the direction of a pin:
// DDRB = pins of PORT B
	//		- Bitn = 1 -> OUTPUT -> the pin send a signal from the microcontroller chip to an outside component (Exemple: the chip send a signal to the led)
	//		- Bitn = 0 -> INPUT (DEFAULT VALUE) -> pin get a signal from an outside components to the microcontroller chip (Exemple: a button send an info to the chip)

// PINX -> register to read a pin Numeric state (HIGH or LOW)
// PIND = pins of PORT D
	//		- not null -> HIGH (realease button)
	//		- 0 -> LOW (pressed button)

//PORTX -> to define the state of a pin (in output):
// PORTB -> pins in output of PORT B
	//		- Bitn = 1 -> HIGH -> the pin give a voltage (close to 5V or 3.3V)
	//		- Bitn = 0 -> LOW (DEFAULT VALUE) -> the pin don't get voltage (GND/ close to 0V)

// *internal pull up -> prevents the input from floating, that could cause wrong readings due to electrical perturbation (tension variation)
// when the button is release, it keeps the pin HIGH, at a volatage close to 5V
// when pressed, the button connests the pin to GND, making it low;

// Page 58-59-60 datasheet
