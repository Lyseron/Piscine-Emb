// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>
// for sei() and ISR
#include <avr/interrupt.h>
// to use boolean
#include <stdbool.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define RED PD5
#define GREEN PD6
#define BLUE PD3

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init_rgb(void);
void uart_init(void);
bool parse_input(const char *str);
void set_rgb(uint8_t r, uint8_t g, uint8_t b);
char uart_rx(void);
void uart_tx(char c);
void uart_printstr(const char *str);
void get_rgb(uint8_t *r, uint8_t *g, uint8_t *b, const char *str);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

int main(void)
{
	// Init RGB
	init_rgb();
	// Init UART
	uart_init();

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	// Contains the color in hex
	char str_input[8];
	uint8_t i = 0;

	while (1)
	{
		// Received a char from the pc to the chips through UART
		char c = uart_rx();

		// If it's newline or a return -> the string is finish execute the program or error
		if (c == '\n' || c == '\r')
		{
			// Send a char to the pc through UART to see what is typed
			uart_tx(c);

			// End proprely the string (to after parse it as a complete string)
			str_input[i] = '\0';

			// If the input is a correct color
			if (parse_input(str_input))
			{
				// Fill the decimal value of each color from string_input
				get_rgb(&r, &g, &b, str_input);
				// Set the RGB LED color by modifying the duty cycle (when a event occur)
				set_rgb(r, g, b);
				// print a newline and put the cursor back at the begining of the line
				uart_printstr("\r\n");
			}
			// else if the input is incorrect
			else
				// Print a error msg if the str is not a valid RGB -> /r -> put the curser back at the begining
				uart_printstr("Error: Invalid RGB\r\n");

			// Restart i at the begining
			i = 0;
			
			str_input[0] = '\0';
		}
		// If it is not the end of the input(enter not press yet)
		else if (i < 7)
		{
			// Fill my string with the input
			str_input[i] = c;

			// Send a char to the pc through UART to see what is typed
			uart_tx(c);
			
			// Advence i to add the next typed char
			i++;
		}
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Confingure Timer0 and Timer2 to generate three PWM signal
void init_rgb(void)
{
	// Configurate the LED RGB as output
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);

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

TCNT1 -> register containing the current value of Timer1

Timer0 -> Automatic counter 0,1,2,3 ect til the overload 6 then back to 0 (16 bit)
	In PMW (Pulse Width Modulation) TOP = 255, BOTTOM = 0;
	when OCR0(canal A or B) is reached -> Timer0 starts again from 0
	
	- TCCR0A -> register used to configure a part of Timer1
		- Bit WGM01 = 1, WGM00 = 1 -> select mode PWM
		- Bit COM0A1 = 1, COM0B1 = 1 -> Non inverted mode HIGH TO LOW on compare match : 0CR0A / OCR0B
			->Non inverted -> at first OC0A or 0C0B is HIGH then becomes LOW

	- TCCR0B -> register used to configure another part of Timer1
		// Bit CS01 = 1, CS00 = 1-> select a prescaler of 64
			prescaler -> devide the microcontroller clock to slow down the clock seen by Timer0
			(because 16 000 000UL is too big to fit in TCNT1 that can count max til 65535)
			- CPU clock = 16MHz -> Timer0 clock = 16MHz / 64(prescaler)

Timer2 -> Automatic counter 0,1,2,3 ect til the overload 6 then back to 0 (16 bit)

	TCCR2B -> register used to configure another part of Timer2
		- Bit WGM21 = 1, WGM20 = 1 -> activate the PWM mode
			PWM -> TOP = 255, BOTTOM = 0;
		- Bit CS22 = 1 -> select a prescaler of 64
			prescaler -> devide the microcontroller clock to slow down the clock seen by Timer2
			(because 16 000 000UL is too big to fit in TCNT1 that can count max til 65535)
			-> CPU clock = 16MHz -> Timer2 clock = 16MHz / 64(prescaler)
		- Bit COM2B1 = 1 -> Non inverted mode HIGH TO LOW on compare match : 0CR0A / OCR0B
			->Non inverted -> at first OC1A is HIGH then becomes LOW

// OCR0A, OCR0B, OCR2B -> Out Compare Register 0 or 2, canal A or B -> Define when OC0A, OC0B or OC2B changes from HIGH to LOW
	// Timer0 or Timer2 compares TCNT1 with OCR0A, OCR0B or OCR2B

// OC0A, OC0B, OC2B -> Special fonction of respectively PD6, PD5, PD3 pin
	//	- OC -> Output Compare
	//	- 0 or 2 -> select Timer0 or Timer2
	// 	- A. B -> select Canal A or B of the Timer
		// Canal A Timer0:
			OC0A -> output pin PD6 -> comparaison match = OCR0A
		- Canal B timer0:
			OC0B -> output pin PD5 -> comparaison match = OCR0B
		- Canal B timer2:
			OC2B -> output pin PD3 -> comparaison match = OCR2B
*/