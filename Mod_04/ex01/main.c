// To know the specific name of the components AVR in the right chip (here atmega328p)
#include <avr/io.h>
// For interrupt utils
#include <avr/interrupt.h>
// For the _delay_ms
#include <util/delay.h>
#include <stdbool.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define LED PB1

// To count the number of interuption
volatile uint8_t interupt_counter = 0;

// To know if the brighness need to go up or down, true = down, up = false
bool brighness_down = false;

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void TIMER0_COMPA_vect(void) __attribute__((signal));
void init(void);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

// Exo -> 
int main(void)
{
	init();

	// Enable global interupt (sei())
	__asm__ __volatile__ ("sei" ::: "memory");
	
	while (1)
	{}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Called when Timer0 reached OCR0A (around every 10ms)
void TIMER0_COMPA_vect(void)
{
	if (brighness_down == false)
	{
		// Increase the counter at every Timer0 interrupt (around 10ms)
		interupt_counter++;

		// At LED full ON, start decreasing
		if (interupt_counter == 50)
			brighness_down = true;
	}
	else
	{
		// Decrease the counter at every Timer0 interrupt (around 10ms)
		interupt_counter--;

		// At LED full OFF, start increasing
		if (interupt_counter == 0)
			brighness_down = false;
	}

	// Convert counter value (0 to 50) into PWM value (0 to 255)
	// Update led brighness depending of the nb of interupt
	OCR1A = interupt_counter * 255 / 50;
}

void init(void)
{
	// Set OUTPUT for the Led and button
	DDRB |= (1 << LED);

// ----------- Timer 0 ------------- //
	// Mode CTC on timer0
	TCCR0A |= (1 << WGM01);

	// Set the prescaler to 1024 -> to slow down the Timer0 clock
	TCCR0B |= (1 << CS00) | (1 << CS02);

	// Interupt around every 10ms
	OCR0A = 155;

	// Enable the interuption on compare match OCR0A
	TIMSK0 |= (1 << OCIE0A);

// ----------- Timer 1 ------------- //
	// Activate the Fast PWM phase correct mode 1 (8bit) -> The 0 is actually 0
	TCCR1A = (1 << WGM10);

	// Set the prescaler to 8 -> to slow down the Timer1 clock
	TCCR1B |= (1 << CS11);

	// Non inverted PMW -> Start HIGH then becomes LOW when OCR1A is reached
	TCCR1A |= (1 << COM1A1);

	// Inital to 0 so LED off at first
	OCR1A = 0;
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


// ---------- TIMER 0 -------------------- //

Timer0 -> 8bit timer
	-> Count from 0 to 255 maximum
	Datasheet page 74
CTC -> Clear Timer on compare match
	-> Timer0 count from 0 to OCR0A
	-> When TCNT0 reaches OCR0A, a compare match occurs
	Datasheet page -> 79

TCCR0A -> Timer0 Control Register A
	-> Controls the operating mode of Timer0
		-> CTC = WGM01 = 1, WGM00 = 0, WGM02 = 0
		Datasheet page -> 84 - 86
TCCR0B -> Timer0 Control Register B
	-> CONtrol the Timer clock and prescaler
		-> Presacaler 1024 -> CS02 = 1, CS01 = 0, CS00 = 0
		-> TCCR0B = 0 -> STop Timer0 because all clock select bit are 0
		Datasheet page -> 86-87

TCNT0 -> Timer0 Register
	-> Contain the current value of timer0 -> from 0 to 255
		-> TCNT0 = 0 -> Reset Timer0 to 0 
		Datasheet page -> 87

OCR0A -> OUtput Compare Register A
	-> COntains the value compared with TCNT0
		-> OCR0A = 155
		Datasheet page 79 and 87

		Timer0 -> 8 bit timer -> max value = 255
	Generate an interupt every 10ms (the interupt that will change the led brighness)
		CTC mode ->
		-> Timer0 count from 0 to OCR0A, then compare match on OCR0A, then back to 0
		-> an interupt is triggered on OCR0A
		-> datasheet page 78 (CTC) and 86 and 88

	-> Prescaler = 1024
		-> 16 000 000 / 1024 = 15 625
			-> Timer0 counts 15 625 times per second
			-> datasheet page 87

Count for 10ms (interupt that changes OCR0A)
	-> 1s = 15 625 count 
	-> 10ms = 0.01
		15 625 * 0.01 = 156.25
			-> So for 10ms -> 156 count
	OCR0A = 156 - 1 (for the 0) = 155

Number of interupt per second
	Timer0 count 15 625 times per second
		-> each interupt needs 156 counts (from 0 to 155)
		-> Since we made 10ms for each interuption and 10ms = 155 count
		15 625 / 156 = 100 interuption
			-> so from 0 to 50 -> OCR1A is going up -> 50 interupt = aproximately 0.5s
			-> fron 51 to 100 -> OCR1A is going dowm -> 100 interupt = aproximately 1s

		So Timer0 can count up to 255 but in CTC here it restarts after 155

		
// ---------- INTERUPT ----------------- //

SREG -> AVR Status Register
	-> I bit(7) -> Global Interuption Enable
	sei() set the I bit to 1
	Datasheet page -> 11 - 16

void TIMER0_COMPA_vect
	-> Interupt fonction for Timer0 Compare match A
	-> Executed when TCNT0 reaches OCR0A
	Datasheet page -> 49 - 88

// ---------- STATUS ------------------- //

TIMSK0 |= (1 << OCIE0A); -> page 88 datasheet

// ---------- TIMER 1 -------------------- //
Timer1 -> 
	To make the led switch on and off very quickly
	Fast PWM Mode 5 -> 8bit PWM phase correct -> 0 to 255 to 0 
		-> Timer1 counts from 0 to 255 (so easier than 14 because top is already at 255 no need to configurate ICR1)
		-> One PWM cycle contains 256 count

	Prescaler = 8 with a weak one timer1 count faster so the pwm signal repeat fast
		-> 16 000 000 / 8 = 2 000 000
			-> Timer1 counts 2 000 000 times per second
	One PWM cycle needs 256 counts so 
		-> 2 000 000 / 256 = 7 812.5
			-> Timer1 makes 7 812.5 PWM cycle of (0 to 256) per second
		datasheet page 110

OCR1A -> controls how long the LED stays ON
	OCR1A = brighness * 255 / 100
		-> Timer0 interupt every 10ms to reach 100% brigness in 0.5s we need 50 interupt 
		-> 50% brighness = 50 * 255 / 100 = 127

	-> Timer1 counts from 0 to 255
	OCR1a = 25 -> LED ON for a short time ->low brightness
	OCR1A = 128 -> LED ON about half the time -> 50 % brightness
	OCR1A = 200 -> LED ON for a longer time -> hight brightness
		-> The biggest OCR1A is the mode the LED stays ON for each PWM cycle so the brighter she look

// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //

TIMER 0 -> 

	Calcultae Timer0 count per second
		-> F_CPU / Prescaler = count per second
		-> 16 000 000 / 1024 = 15 525
		
	Calculate the number of counts for 10ms
		->count per second Timer0 * 10ms in s = counts for 10ms
		-> 15 625 * 0.1 = 156.25 count per second (156 minus 1 for the 0) = 155

	Calcultae OCR0A
		-> count per secon for 10ms - 1 for the 0
		-> 156 - 1 = 155
		-> Timer0 count from 0 to 155 in CTC mode (then interupt)

	Calculate the number of interrupts per second
		-> count per second / OCR0A
		-> 15 625 / 156 = 100
	-> Timer0 generate 100 interput per second

TIMER 1 -> 

	OCR1A = BRIGHNESS * 255 / 100
		-> convert a pourcentage into a aproximate PWM value
		- Calculate OCR1A
			OCR1A = interupt_counter * 255 / 50
				-> Increases OCR1A from 0 to 255 in 50 interupts.
				-> Decrease OCR1A from 255 to 0 in 50 interput


*/