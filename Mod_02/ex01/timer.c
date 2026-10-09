// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>
// for sei() and ISR
#include <avr/interrupt.h>

void init_timer1()
{
// Value Timer1 needs to reach before starting again from 0 (2s)
	OCR1A = 31249;

// Activate the CTC mode -> Timer1 goes back to 0 when OCR1A is reached
	TCCR1B = (1 << WGM12);

// Set the prescaler to 1024 -> to slow down the Timer1 clock
	TCCR1B |= (1 << CS12) | (1 << CS10);

// Activate the interuption on timer1
	TIMSK1 |= (1 << OCIE1A);
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*
- Timer1 -> Automatic counter 0,1,2,3 ect til the overload 65535 then back to 0 (16 bit)
	-> In CTC mode, it count until OCR1A is reached then starts gaain from 0
- TCCR1B -> register used to configure another part of Timer1
		- Bit WGM12 = 1 ->  activate mode CTC (Clear Timer on Compare Match)
		- Bit CS12 = 1, CS11 = 0, CS10 = 0 -> select a prescaler of 1024
			- prescaler -> devide the microcontroller clock to slow down the clock seen by Timer1 
			- (because 16 000 000UL is too big to fit in TCNT1 that can count max til 65535)
				-> CPU clock = 16MHz
				-> Timer1 clock = 16MHz / 1054(prescaler)

TIMSK1 -> Register that enable interuption
	-> Bit OCIE1A = 1-> activate the compare match canal A on timer1
	-> Bit OCIE1A = 0 -> disable the compare match canal A on timer1
	-> The interupt can execute when TCNT1 match OCR1A
	-> Required global interupt to be activated (sei())
	Datasheet -> page 112

ISR(TIMER1_COMPA_vect) -> Interupt Service Routine
	-> ISR=> automatic fonction executer when a interuption occurs
	-> TIMER1_COMPA_vect -> identifie the interuption of canal A (OCR1A) of timer1
	Datasheet -> page 49-50
sei() -> function that to autorise the global interuption

// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //

// To calculate the stopping time 1Hertz/(0.5)s
	// 16 000 000(CPU frequency) / 1024(prescaler) = 15625 par second
	// 15625 * 2(for 2 second) = 31250
	// 31250 - 1(for the 0) = 31249

*/