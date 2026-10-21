// To know the specific name of the components AVR in the right chip (here atmega328p)
#include <avr/io.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define RED PD5
#define GREEN PD6
#define BLUE PD3

#define D1 PB0
#define D2 PB1
#define D3 PB2
#define D4 PB4

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

void led_init(void)
{
	// Configurate the LED RGB as output
	DDRD |= (1 << BLUE) | (1 << RED) | (1 << GREEN);

	// Configurate the LED D1, D2, D3, D4 as output
	DDRB |= (1 << D1) | (1 << D2) | (1 << D3) | (1 << D4);
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

// Pourcentage of 255 that trun on different led
//	-> 64 = 25% of 255, 128 = 50 % of 255, 192 = 75% of 255, 255 = 100% of 255
void led_jauge(uint8_t value)
{
	// 25 % of 255
	if (value >= 64)
		PORTB |= (1 << D1);
	else
		PORTB &= ~(1 << D1);

	// 50 % of 255
	if (value >= 128)
		PORTB |= (1 << D2);
	else
		PORTB &= ~(1 << D2);

	// 75 % of 255
	if (value >= 192)
		PORTB |= (1 << D3);
	else
		PORTB &= ~(1 << D3);

	// 100 % of 255
	if (value >= 255)
		PORTB |= (1 << D4);
	else
		PORTB &= ~(1 << D4);
}

// Confingure Timer0 and Timer2 to generate three PWM signal
void init_rgb(void)
{
	// Configurate the LED RGB as output
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);

	// Activate the PWM mode 3 (Top = 255)
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

*/