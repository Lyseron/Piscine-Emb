// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>

int main(void)
{
// PORT B -> groups physics of pin (from 0 to 7), each bit (pin) has it's own role
	// PB0 -> bit 0 in PORT B = LED D1 in schematics

// DDRX = Data Direction Register -> to defined the direction of a pin:
// DDRB = pins of PORT B
	//		- Bitn = 1 -> OUTPUT -> the pin send a signal from the microcontroller chip to an outside component (Exemple: the chip send a signal to the led)
	//		- Bitn = 0 -> INPUT (DEFAULT VALUE) -> pin get a signal from an outside components to the microcontroller chip (Exemple: a button send an info to the chip)
//	|= changed just the concern bit to 1, so PB0 in OUTPUT
	DDRB |= (1 << PB0);

//PORTX -> register to define the state of a pin (in output) : page 58 datasheet 
// PORTB -> pins in output of PORT B
	//		- Bitn = 1 -> HIGH -> the pin give a voltage (close to 5V or 3.3V)
	//		- Bitn = 0 -> LOW (DEFAULT VALUE) -> the pin don't get voltage (GND/ 0V)
//	|= changed just the concern bit to 1, so PB0 in HIGH
	PORTB |= (1 << PB0);

// to continously run the program so the led don't turn off, PB0 remains HIGH
// (possible with a delay as well but stops at therm)
	while (1) {}
}




