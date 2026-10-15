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
	// Activate global interupts
	sei();
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
	// Put the LED is OUTPUT
	DDRB |= (1 << LED_D1) | (1 << LED_D2) | (1 << LED_D3) | (1 << LED_D4);

	// Put the SW1(PD2) and SW2(PD4) button to INPUT
	DDRD &= ~(1 << BUTTON_ADD);
	DDRD &= ~(1 << BUTTON_REMOVE);

	// ACtivate internal pull_up page 58 - 73
	PORTD |= (1 << BUTTON_ADD) | (1 << BUTTON_REMOVE);

	// Interuption on descending front (1 -> 0) (HIGH TO LOW)for BUTTON ADD (SW1)
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);

	// Autorize interuption INTO
	EIMSK |= (1 << INT0);

	// Autorize interuption on group PCINT
	PCICR |= (1 << PCIE2);

	// Activate the interuption on PD4 (PCINT20) BUTTON SW2
	PCMSK2 |= (1 << PCINT20);

	// Timer1, CTC mode no prescaler so the timer is stopped
	TCCR1B |= (1 << WGM12);

	// Value to reached
	OCR1A = REACHING_VALUE_TIMER1;

	// Autorize interuption when OCR1A is reached
	TIMSK1 |= (1 << OCIE1A);

	// for the initial value of 0 -> display nothing
	turnLedON();

}

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

void start_timer(void)
{
	// Desactivate the interuption INTO during debounce
	EIMSK &= ~(1 << INT0);
	// Desactivate the interuption on PD4 (PCINT20) duting debounce
	PCMSK2 &= ~(1 << PCINT20);

	// PUt the counter back to 0
	TCNT1 = 0;

	// Set 1024 prescaler on timer1 and start timer1
	TCCR1B |= (1 << CS12) | (1 << CS10);
	TCCR1B &= ~(1<< CS11);
}


// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*

Interupt vector -> page 49-50



// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //



*/