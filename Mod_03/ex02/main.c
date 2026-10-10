// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>
// for sei() and ISR
#include <avr/interrupt.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define RED PD5
#define GREEN PD6
#define BLUE PD3

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init_rgb(void);
void set_rgb(uint8_t r, uint8_t g, uint8_t b);
void wheel(uint8_t pos);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

int main(void)
{
	// Init RGB pins, prescalers, timers, and mode PWM
	init_rgb();

	while (1)
	{
		// Gradually change color through 255 positions
		for (uint8_t i = 0; i < 255; i++)
		{
			wheel(i);
			// slow down the transition a bit to make it prettier
			_delay_ms(10);
		}
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Confingure Timer0 and Timer2 to generate three PWM signal
void init_rgb(void)
{
	// Configurate the LED RGB as output
	DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6);

	// Activate the Fast PWM mode 3 (Top = 255)
		// Timer 0 ->
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
		// Timer 2 ->
	TCCR2A |= (1 << WGM21) | (1 << WGM20);

	// Set the prescaler to 64 -> to slow down the Timers clock
		// Timer 0 ->
	TCCR0B |= (1 << CS01) | (1 << CS00);
		// Timer 2 ->
	TCCR2B |= (1 << CS22);

	// Non inverted PMW -> Start HIGH at bottom (0) then becomes LOW on compare match
		// Timer0 for PD6 and PD5 -> compare match : 0CR0A / OCR0B
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
		// Timer 2 for PD3 -> compare match 0CR2B
	TCCR2A |= (1 << COM2B1);

	// Init start to count at 0
	// PD5 -> RED
	OCR0B = 0;
	// PD6 -> GREEN
	OCR0A = 0;
	// PD3 -> BLUE
	OCR2B = 0;
}

// Set the RGB LED color by modifying the duty cycle (when a event occur)
void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	// RED -> Timer0 channel B -> interact with PD5
	OCR0B = r;
	// GREEN -> Timer0 channel A -> interact with PD6
	OCR0A = g;
	// BLUE -> Timer2 channel B -> interact with PD3
	OCR2B = b;
}

// Generate a gradient using two primary color at the time
void wheel(uint8_t pos) 
{
	pos = 255 - pos;

	if (pos < 85)
		set_rgb(255 - pos * 3, 0, pos * 3);
	else if (pos < 170) 
	{
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} 
	else 
	{
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
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

// TCNT1 -> register containing the current value of Timer1

// Timer1 -> Automatic counter 0,1,2,3 ect til the overload 6 then back to 0 (16 bit)
	// In Fast PMW (Pulse Width Modulation), Timer1 count from 0 to ICR1
	// when ICR1 is reached -> Timer1 starts again from 0

	// ICR1 -> Max Value of Timer1
		// define the total duration of one PWM period
	
	// TCCR1A -> register used to configure a part of Timer1
		// Bit WGM11 = 1 -> with WGM12 and WGM13 to select mode PWM
		// Bit COM1A1/COM1A0 -> control what to do on the OC1A(PB1) pin
		// Bit COM1A0(6) = 0
		// Bit COM1A1(7) = 1
			// Non inverted -> at first OC1A is HIGH then becomes LOW

	// TCCR1B -> register used to configure another part of Timer1
		// Bit WGM10 = 0, WGM11 = 1, WGM12 = 1, WGM12 = 1 -> activate the PWM mode
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
*/