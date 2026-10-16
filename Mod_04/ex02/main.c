// To know the specific name of the components AVR in the right chip (here atmega328p)
#include <avr/io.h>
// For interrupt utils
#include <avr/interrupt.h>
#include <stdbool.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define LED_D1 PB0
#define LED_D2 PB1
#define LED_D3 PB2
#define LED_D4 PB4

#define BUTTON_ADD PD2
#define BUTTON_REMOVE PD4

// 16 000 000 / 1024 = 15 625 per seconde -> for 20ms -> 15 625 * 0.02 = 312.5
#define REACHING_VALUE_TIMER1 312

// For the number to be display
volatile uint8_t counter = 0;

// To check if the button was pressed
volatile bool SW1_pressed = false;
volatile bool SW2_pressed = false;

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init(void);
void start_timer(void);
void turnLedON(void);

void INT0_vect(void) __attribute((signal));
void PCINT2_vect(void) __attribute((signal));
void TIMER1_COMPA_vect(void) __attribute((signal));

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

// Exo -> 
int main(void)
{
	// Configurate Led, button and interuption
	init();
	// Enable global interupt (sei())
	__asm__ __volatile__ ("sei" ::: "memory");
	// Useless loop
	while (1)
	{}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Interuption for when the button_add(SW1) is pressed 
void INT0_vect(void)
{
	// Start the 20ms debounce timer
	start_timer();
}

// Interuption for when the button_remove(SW2) is pressed 
void PCINT2_vect(void)
{
	// Start the 20ms debounce timer
	start_timer();
}

// Executed when TCNT1 reches OCR1A (20ms)
// Stop timer1 when both buttons are released
void TIMER1_COMPA_vect(void)
{
	// Check if SW1 is pressed after 20ms
	if (!(PIND & (1 << BUTTON_ADD)))
	{
		// if the button1 was not pressed
		if (SW1_pressed == false)
		{
			// change the status to pressed
			SW1_pressed = true;
			// ADD 1 to the number to be display
			counter++;
			// if it's above 15 -> back to 0
			if (counter > 15)
				counter = 0;
			// then turn the right led on
			turnLedON();
		}
	}
	// else if it's already pressed -> set the state to not press
	else
		SW1_pressed = false;

	// Check SW1 after 20ms
	if (!(PIND & (1 << BUTTON_REMOVE)))
	{
		// if the button2 was not pressed
		if (SW2_pressed == false)
		{
			// change the status to pressed
			SW2_pressed = true;
			// if it's equal to 0 -> back to 15
			if (counter == 0)
				counter = 15;
			// otherwise remove 1 to the number to be display
			else
				counter--;
			// then turn the right led on
			turnLedON();
		}	
	}
	else
			SW2_pressed = false;

	// if both button are not pressed -> stop timer 1
	if (SW1_pressed == false && SW2_pressed == false)
	{
		// Stop Timer1 -> clear all Timer1 clock 
		TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));

		// Clear pending interupt flags
		// SW1 -> 
		EIFR = (1 << INTF0);
		// DW2 -> 
		PCIFR = (1 << PCIF2);

		// Reactivate the interuption on the buttons
		// SW1 -> 
		EIMSK |= (1 << INT0);
		// SW2 -> 
		PCMSK2 |= (1 << PCINT20);
	}
}

void init(void)
{
	// Put the LED as OUTPUT
	DDRB |= (1 << LED_D1) | (1 << LED_D2) | (1 << LED_D3) | (1 << LED_D4);

	// Put the SW1(PD2) and SW2(PD4) button as INPUT
	DDRD &= ~(1 << BUTTON_ADD);
	DDRD &= ~(1 << BUTTON_REMOVE);

	// Activate internal pull_up page 58 - 73
	PORTD |= (1 << BUTTON_ADD) | (1 << BUTTON_REMOVE);

	// ---------- External interrupt SW1 -------- //
	// Interuption on descending front for BUTTON ADD (SW1) press -> interuption, release -> no interuption
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);

	// Autorize interuption INTO fro SW1
	EIMSK |= (1 << INT0);

	// ---------- External interupt SW2 -------- //

	// Autorize interuption on group PCINT for SW2
	PCICR |= (1 << PCIE2);

	// Activate the interuption on PD4 (PCINT20) BUTTON SW2
	PCMSK2 |= (1 << PCINT20);

	// ---------- Timer 1 ---------------------- //

	// Timer1, CTC mode no prescaler yet so the timer is stopped
	TCCR1B |= (1 << WGM12);

	// Compare match around every 20ms
	OCR1A = REACHING_VALUE_TIMER1;

	// Enable interuption when OCR1A is reached (compare match a)
	TIMSK1 |= (1 << OCIE1A);

	// for the initial value of 0 -> display nothing
	turnLedON();

}

// Display counter as a 4 bit binary nb
void turnLedON(void)
{
	// Tuning off all the led
	PORTB &= ~((1 << LED_D1) | (1 << LED_D2) |(1 << LED_D3) |(1 << LED_D4));

	// Turn on the led if the bit looked is 1 (only the last 3 bit)
	// for the bit 0 -> turn D1 on
	if (counter & (1 << 0))
		PORTB |= (1 << LED_D1);

	// for the bit 1 -> turn D2 on
	if (counter & (1 << 1))
		PORTB |= (1 << LED_D2);

	// for the bit 2 -> turn D3 on
	if (counter & (1 << 2))
		PORTB |= (1 << LED_D3);

	// for the bit 3 -> turn D4 on
	// bit 3 must be display on PB4 not PB3
	if (counter & (1 << 3))
		PORTB |= (1 << LED_D4);
}

// Start Timer1 to check button states after 20ms
void start_timer(void)
{
	// Desactivate the interuption INTO during debounce SW1
	EIMSK &= ~(1 << INT0);

	// Desactivate the interuption on PD4 (PCINT20) during debounce SW2
	PCMSK2 &= ~(1 << PCINT20);

	// Put the counter back to 0
	TCNT1 = 0;

	// Set 1024 prescaler on timer1 and start timer1
	TCCR1B |= (1 << CS12) | (1 << CS10);
	TCCR1B &= ~(1<< CS11);
}


// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*

Interupt vector -> page 49-50
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

PIND -> 
	Datasheet page -> 73

// ---------- BUTTONS ----------- //

SW1 -> Button ADD
	-> COnnected to PD2 -> Use INT0 as external interupt
	Datasheet page -> 53 to 70 - 71

DW2 -> Button Remove
	-> Connected to PD4 -> Use PCINT20
	-> Use a pin change interupt
		-> PCINT20 detect logical changes
			-> HIGH to LOW -> button pressed
			-> LOW to HIGH -> button released
	Datasheet page 53, 56-57 and 70

// ---------- INTERUPT ----------------- //

EICRA -> External Interrupt Control Register Canal A
	-> Defines what event triggers an externals interrupt
	->Bit ICS01(1) = 1, ISC00 = 0 -> INT0 is triggered on the falling edge (when button is pressed)
	Datasheet page -> 54

EIMSK -> External Interrupt Mask Register
	-> Enables or disable individual external interrupts
	-> INT0 (bit 0)
		-> Bit = 1 -> INT- enabled
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


PCINT -> 



// ---------- TIMER 1 ----------------- //




// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //



*/