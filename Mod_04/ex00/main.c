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
volatile uint8_t interupt_counter = 0;

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init(void);
void start_timer(void);
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

void INT0_vect(void)
{
	start_timer();
}

void TIMER0_COMPA_vect(void)
{
	interupt_counter++;
	// around 20ms
	if (interupt_counter == 2)
	{
		// Clear waiting interuption
		EIFR = (1 << INTF0);

		// Enable interupt
		EIMSK |= (1 << INT0);
		EIMSK &= ~(1 << INT1);

		TCNT0 = 0;

		
		interupt_counter = 0;
	}
}

void init(void)
{
	// Set OUTPUT for the Led and button
	DDRB |= (1 << LED);
	DDRD &= ~(1 << BUTTON);

	// For the internal pull up
	PORTD |= (1 << BUTTON);

	// Descending front for the button generates an interupt request (when press -> interuption)
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);

	// Clear waiting interuption
	EIFR = (1 << INTF0);

	// Configurate the external interupt for the button INT0
	EIMSK |= (1 << INT0);
	EIMSK &= ~(1 << INT1);

	// ----------- Timer 0 ------------- //
	// Mode CTC on timer0
	TCCR0A |= (1 << WGM01);

	// Interupt around every 10ms
	OCR0A = 155;


	// Autorize the interuption on compare match OCR0A
	TIMSK0 |= (1 << OCIE0A);

}

void start_timer(void)
{
	TCCR0A |= (1 << WGM01);
	// Desactivate the interuption on the buttonSW1
	EIMSK &= ~((1 << INT0));

// Toggle the led
		PORTB ^= (1 << LED);

	// Reset the timer at 0
	TCNT0 = 0;
	// Set the prescaler to 1024 -> to slow down the Timer0 clock
	TCCR0B |= (1 << CS00) | (1 << CS02);
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