// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>

// Ex01 -> toggle the Led(PB1) every 1Hertz
int main(void)
{
// Put PB1 in Output mode
	DDRB |= (1 << PB1);

// Activate the CTC mode -> Timer1 goes back to 0 when OCR1A is reached
	TCCR1B = (1 << WGM12);
// Set the prescaler to 256 -> to slow down the Timer1 clock
	TCCR1B |= (1 << CS12);

// Value Timer1 needs to reach before starting again from 0 (here 0.5s)
	OCR1A = 31249;

// Toggle OC1A(PB1) each time Timer1 reaches OCR1A -> HIGH to LOW or LOW to HIGH
	TCCR1A = (1 << COM1A0);

	while (1) {}
}

// DDRX = Data Direction Register -> to defined the direction of a pin:
	// DDRB = pins of PORT B
		//	- Bitn = 1 -> OUTPUT -> the pin send a signal from the microcontroller chip to an outside component 
			// Exemple: the chip send a signal to the led
		//	- Bitn = 0 -> INPUT (DEFAULT VALUE) -> pin get a signal from an outside components to the microcontroller chip 
			// Exemple: a button send an info to the chip

// Timer1 -> Automatic counter 0,1,2,3 ect til the overload 65535 then back to 0 (16 bit)
	// In CTC mode, it count until OCR1A is reached then starts gaain from 0

// TCNT1 -> register containing the current value of Timer1
	
	// TCCR1A -> register used to configure a part of Timer1
		// Bit COM1A0/COM1A0 -> control what happens on the OC1A(PB1) pin
		// Bit COM1A0 = 1
		// Bit COM1A1 = 0
			// Toggle OC1A when OCR1A is reached
		

	// TCCR1B -> register used to configure another part of Timer1
		// Bit WGM12 = 1 ->  activate mode CTC (Clear Timer on Compare Match)
		// Bit CS12 = 1, CS11 = 0, CS10 = 0 -> select a prescaler of 256
			// prescaler -> devide the microcontroller clock to slow down the clock seen by Timer1 
			// (because 16 000 000UL is too big to fit in TCNT1 that can count max til 65535)
				// CPU clock = 16MHz
				// Timer1 clock = 16MHz / 256(prescaler)

// OCR1A -> Out Compare Register 1 A -> contain the value that Timer1 must reach
	// Timer1 compares TCNT1 with OCR1A

// OC1A -> Special fonction of the PB1 pin
	//	- OC -> Output Compare
	//	- 1 -> select Timer1
	// 	- A -> select Canal A of the Timer1
		// Canal A -> For OC1A(PB1) or OC1B(PB2)

// To calculate the stopping time 1Hertz/(0.5)s
	// 16 000 000(CPU frequency) / 256(prescaler) = 62 500 par second
	// 62 500 / 2(for 0.5 second ou 1hertz) = 31250
	// 31250 - 1(for the 0) = 31249
