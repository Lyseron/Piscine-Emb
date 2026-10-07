// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>

// Ex02 -> turn the Led on 10% evry 1Hertz(0.5s) and off the remaining 90%
int main(void)
{
// Put OC1A(PB1) in Output mode
	DDRB |= (1 << PB1);

// Set the prescaler to 256 -> to slow down the Timer1 clock
	TCCR1B = (1 << CS12);

// Activate the Fast PWM mode 14 ->  
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	TCCR1A = (1 << WGM11);

// Value Timer1 needs to reach before starting again from 0 (here 0.5s)
	ICR1 = 62499;

// How long the Led needs to stays on (10% of 1Hertz)
	OCR1A = 6250;

// Non inverted PMW -> Start HIGH then becomes LOW when OCR1A is reached
// Give Timer1 access to output OC1A(PB1) -> Bit COM1A1 and COM1A0 defined the behavior of OCA1
	TCCR1A |= (1 << COM1A1);

	while (1) {}
}

// DDRX = Data Direction Register -> to defined the direction of a pin:
	// DDRB = pins of PORT B
		//	- Bitn = 1 -> OUTPUT -> the pin send a signal from the microcontroller chip to an outside component 
			// Exemple: the chip send a signal to the led
		//	- Bitn = 0 -> INPUT (DEFAULT VALUE) -> pin get a signal from an outside components to the microcontroller chip 
			// Exemple: a button send an info to the chip

// TCNT1 -> register containing the current value of Timer1

// Timer1 -> Automatic counter 0,1,2,3 ect til the overload 6 then back to 0 (16 bit)
	// In Fast PMW (Pulse Width Modulation), Timer1 count from 0 to ICR1
	// when ICR1 is reached -> Timer1 starts again from 0

	// ICR1 -> Max Value of Timer1
		// define the total duration of one PWM period
	
	// TCCR1A -> register used to configure a part of Timer1
		// Bit WGM11 = 1 -> with WGM12 and WGM13 to select Fast PWM mode 14
		// Bit COM1A1/COM1A0 -> control what to do on the OC1A(PB1) pin
		// Bit COM1A0(6) = 0
		// Bit COM1A1(7) = 1
			// Non inverted -> at first OC1A is HIGH then becomes LOW

	// TCCR1B -> register used to configure another part of Timer1
		// Bit WGM10 = 0, WGM11 = 1, WGM12 = 1, WGM12 = 1 -> activate the fast PWM mode
			// Fast PWM -> 
		// Bit CS12 = 1, CS11 = 0, CS10 = 0 -> select a prescaler of 256
			// prescaler -> devide the microcontroller clock to slow down the clock seen by Timer1 
			// (because 16 000 000UL is too big to fit in TCNT1 that can count max til 65535)
				// CPU clock = 16MHz
				// Timer1 clock = 16MHz / 256(prescaler)

// OCR1A -> Out Compare Register 1 A -> Define when OC1A changes from HIGH to LOW
	// Timer1 compares TCNT1 with OCR1A

// OC1A -> Special fonction of the PB1 pin
	//	- OC -> Output Compare
	//	- 1 -> select Timer1
	// 	- A -> select Canal A of the Timer1
		// Canal A :
			// OC1A(PB1) -> output pin PB1
			// OCR1A -> -> comparaison value

// To calculate the stopping time 1Hertz/(0.5)s
	// 16 000 000(CPU frequency) / 256(prescaler) = 62 500 par second
	// 62 500 / 2(for 0.5 second ou 1hertz) = 31250
	// 31250 - 1(for the 0) = 31249

// To calculate nb to stop the HIGH
	// 16 000 000(CPU frequency) / 256(prescaler) = 62 500 par second
	// 62500 / 10 (for 10%) = 6250
	// HIGH -> 0.1s
	// LOW -> 0.9s
