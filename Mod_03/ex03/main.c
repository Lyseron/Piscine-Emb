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

#define F_CPU 16000000UL
#define UART_BAUDRATE 115200
#define REGISTER_FOR_TRANSMISSION_SPEED (F_CPU / (8 * UART_BAUDRATE) - 1)

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

	char str_input[8];
	uint8_t i = 0;

	while (1)
	{
		char c = uart_rx();
		if (c == '\n' || c == '\r')
		{
			uart_tx(c);
			str_input[i] = '\0';
			if (parse_input(str_input))
			{
				get_rgb(&r, &g, &b, str_input);
				set_rgb(r, g, b);
				uart_printstr("\r\n");
			}
			else
				// print a error msg if the str is not a valid RGB -> /r -> put the curser back at the begining
				uart_printstr("Error: Invalid RGB\r\n");
			i = 0;
			str_input[0] = '\0';
		}
		else if (i < 7)
		{
			str_input[i] = c;
			uart_tx(c);
			i++;
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

// configurate UART (transmission speed, nb of bit, parity, stopping bit, emmetor activation)
void uart_init(void)
{
	// Configure the baud rate register
	UBRR0L = REGISTER_FOR_TRANSMISSION_SPEED;

	// Activate the transmission & reception (TXEN0 = 1, RXEN0 = 1)
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);

	// Define the data size as 8 bit (UCSZ01 = 1, UCSZ00 = 1)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	// activate the asynchronous double speed mode (U2X0 = 1)
	UCSR0A |= (1 << U2X0);
}

// Received a char from the pc to the chips through UART
char uart_rx(void)
{
	// Wait until there is a char receive then retrun it
	while ((UCSR0A & (1 << RXC0)) == 0)
	{}
	return (UDR0);
}

// Send a char to the pc through UART
void uart_tx(char c)
{
	// Wait until URDE0 is set to 1 (transmit data register ready)
	while ((UCSR0A & (1 << UDRE0)) == 0)
	{}

	// Write the char into the UART data register
	UDR0 = c;
}

// send the string, one char at the time to the pc through UART
void uart_printstr(const char *str)
{
	int i = 0;
	while (str[i])
	{
		uart_tx(str[i]);
		i++;
	}
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

bool is_good_value(char c)
{
	if (c >= '0' && c <= '9')
		return (true);
	else if (c >= 'A' && c <= 'F')
		return (true);
	else
		return (false);
}

uint8_t size(const char *str)
{
	uint8_t i = 0;
	while (str[i])
		i++;
	return (i);
}

bool parse_input(const char *str)
{
	if (str[0] != '#')
		return (false);
	if (size(str) != 7)
		return (false);
	for (uint8_t i = 1; i < 7; i++)
		if (is_good_value(str[i]) == false)
			return (false);
	return (true);
}

uint8_t hex_value(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else
		return (c - 'A' + 10);
}

uint8_t convert_hex_to_decimal(char first_nb, char second_nb)
{
	return (hex_value((first_nb)) * 16 + hex_value((second_nb)));
}

void get_rgb(uint8_t *r, uint8_t *g, uint8_t *b, const char *str)
{
	*r = convert_hex_to_decimal(str[1], str[2]);
	*g = convert_hex_to_decimal(str[3], str[4]);
	*b = convert_hex_to_decimal(str[5], str[6]);
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*
	Timer0 ->page 86 datasheet fast pwnm
	timer2 ->page 129

	// DDRX = Data Direction Register -> to defined the direction of a pin:
	// DDRD = pins of PORT D
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
*/