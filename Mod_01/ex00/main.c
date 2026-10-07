// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>

// Ex00 -> toggle the Led(PB1) every 1Hertz without using hardware

int main(void)
{
//	|= changed just the concern bit to 1, so PB1 in OUTPUT (the microcontroler send a signal to the component)
	DDRB |= (1 << PB1);

	while (1)
	{
	// wait very approximatively 0.5s
		for (volatile uint32_t i = 0; i < 235294; i++)
			{}

	// toggle PB1 so the led can be on or off
		PORTB ^= (1 << PB1);
	}
}

// PORT B -> groups physics of pin (from PB0 to PB7), each bit correspond to a pin and has it's own role
	// PB1 -> bit 1 in PORT B = LED D1 in schematics

// DDRX = Data Direction Register -> to defined the direction of a pin:
// DDRB = pins of PORT B
	//		- Bitn = 1 -> OUTPUT -> the pin send a signal from the microcontroller chip to an outside component (Exemple: the chip send a signal to the led)
	//		- Bitn = 0 -> INPUT (DEFAULT VALUE) -> pin get a signal from an outside components to the microcontroller chip (Exemple: a button send an info to the chip)

//PORTX -> to define the state of a pin (in output):
// PORTB -> pins in output of PORT B
	//		- Bitn = 1 -> HIGH -> the pin give a voltage (close to 5V or 3.3V)
	//		- Bitn = 0 -> LOW (DEFAULT VALUE) -> the pin don't get voltage (GND/ close to 0V)

// volatile -> in front of a variable tell the compiler to not optimize but consider my code as it is
	// important in empty loop otherwize the compiler can considere it empty and skip it
	
// calculate the time to wait:
	// CPU = 16 000 000UL cycle/seconde
	// for 0.5s -> 16 000 000 * 0.5 = 8 000 000
	// so the processor needs to be occupied for 8 000 000 cycle/0.5s
	// test of for (volatile uint32_t i = 0; i < 1000000; i++) with reading of the .s and additioning all cycle
	// for 1 000 000 iteration -> cycle = ~34 000 000 cycle(ask IA cause it's long to count them by hand)
	// so // 1 iteration of the loop take ~34 CPU cycle
	// CPU = 16 000 000 cycle/second
	// for 0.5s -> 16 000 000(1s) / 2 = 8 000 000
	// so 8 000 000 / 34 = ~235 294
