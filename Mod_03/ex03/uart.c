// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define F_CPU 16000000UL
#define UART_BAUDRATE 115200
#define REGISTER_FOR_TRANSMISSION_SPEED (F_CPU / (8 * UART_BAUDRATE) - 1)

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

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

// ------------------------------------------- NOTES ---------------------------------------------------------- //


// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //
