// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>
// for sei() and ISR
#include <avr/interrupt.h>

// ------------------------------------------- DEFINE --------------------------------------------------------- //

#define F_CPU 16000000UL
#define UART_BAUDRATE 115200
#define REGISTER_FOR_TRANSMISSION_SPEED (F_CPU / (8 * UART_BAUDRATE) - 1)

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void uart_init(void);
void uart_tx(char c);
char changes_maj_min(char c);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

// Ex03: Received a char form the PC and send it back to the PC but with the maj <-> min change
	// Configuration: 115200 baud, 8N1
int main(void)
{
	// init the UART
	uart_init();

	// activate the possibility to uses unteruptions (global interuption)
	sei();

	// useless infinite loop
	while (1)
	{}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Interuption at reception of data
ISR(USART_RX_vect)
{
	// receive a char from the pc to the chip
	char c = UDR0;

	// convert a char min to a maj and a maj to a min
	char convert_c = changes_maj_min(c);

	// transmit a char from to chip to the pc
	uart_tx(convert_c); 
}

// configurate UART (transmission speed, nb of bit, parity, stopping bit, emmetor activation)
void uart_init(void)
{
	// Configure the baud rate register
	UBRR0L = REGISTER_FOR_TRANSMISSION_SPEED;

	// Activate the transmission & reception (TXEN0 = 1, RXEN0 = 1)
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);

	// Activate the reception of intrupt
	UCSR0B |= (1 << RXCIE0);

	// Define the data size as 8 bit (UCSZ01 = 1, UCSZ00 = 1)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	// activate the asynchronous double speed mode (U2X0 = 1)
	UCSR0A |= (1 << U2X0);
}

// Send a char to the pc from the chip through UART
void uart_tx(char c)
{
	// Wait until URDE0 is set to 1 (transmit data register ready)
	while ((UCSR0A & (1 << UDRE0)) == 0)
	{}

	// Write the char into the UART data register
	UDR0 = c;
}

char changes_maj_min(char c)
{
	if (c >= 'A' && c <= 'Z')
		c += 32;
	else if (c >= 'a' && c <= 'z')
		c -= 32;
	return (c);
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //

/*
F_CPU -> Frequency of the microcontrolleur clock in Hz (16MHz = 16 000 000UL)

UART_BAUDRATE -> Desired UART transmission speed
	- 11500 -> Desired transmission speed in bauds
	- Baud rate -> Number of symbols(1 symbols = 1bit) transmitted per second
	- At 115200 baud -> ~115200 bits per second

UART -> Universal Asynchronous Receiver/Transmitter (communication between the microcontroller and the pc)
	- Asynchronous -> The UART and PC do not share a clock signal, but both devices must uses aproximativly the same baud rate and data format
		- The start bit synchronize the receiver for each char
	- TX -> Transmit Data
	- RX -> Receive data
	- UART uses the internal clock of the microcontroller to determine the transmission speed
	- 8N1 -> format of data (8 data bit, N = No parity, 1 = one stop bit)
		- Start bit -> Indicate the begining of a char (0)
		- Data bits -> 8 bit containing the char
			- Exemple ('Z' -> ACSII = 90, 90 en binaire = 01011010)
		- Parity -> Optional error-detection bit (disable here)
		- Stop bit -> Indicates the end of the data
		- Total -> 10 bits transmitted for each char

UBRR0 -> USART Baud Rate Register 0
	- Used to configure the UART transmission speed from the microcontroller clock
	- Contains the value used to divide the clock frequency
	- UBRR0 is divided into two registers (16 bit):
		- UBRR0H -> High byte of the baud rate register
		- UBRR0L -> Low byte of the baud rate register
	- The baud rate is calculated using the value stored in UBRR0
	the value contain in UBRR0 will be used in the formula -> 16 x (UBRR0 + 1) to get the divider for baud calcul
	- baud = F_CPU / (16 x (UBRR0 + 1)) 
	- to divide the cpu by the result of this formula to get as close as possible to the right baud rate
	- when U2X0 bit of the UCSR0A register is activated (bit put at 1) -> the value 16 in the formula is changed to 8
	- new formula -> baud = F_CPU / (8 x (UBRR0 + 1))

- UCSR0A -> UART Control and Status Register A (state/ flags)
	- Contains UART status flags ans the double speed mode configuration
	
	- Bit U2X0(1) -> Double Transmission Speed
		- U2X0 = 0 -> Normal Asynchronous mode (division by 16)
		- U2X0 = 1 -> Double speed Asynchronous mode (division by 8)
		- Changes how the UART uses the microcontroller clock

	- Bit UDRE0(5) -> USART Data Register Empty
		- Bit that indicate that the register of data is ready to receive a new char
			- if non null(1) -> ready to receive data
			- il null(0) -> not ready
		- UDR0 -> Register where we write the char to send (life a buffer) (see UDRO section down)

	- Bit RXC0(7) -> USART Receive Complete
		-> RXC0 = 0 -> No unread recceived data available
		-> RXC0 = 1 -> Received data is available in UDR0
		- Used when receiving charactere from pc

UCSR0B -> UART Control and Status Register B (transmission , reception and interupt)
	- Used to activate transmission, reception and UART interrupts
	
	- Bit TXEN0(3) -> Activate the transmission UART
		- TXEN0 = 0 -> Disable transmission
		- TXEN0 = 1 -> Activate transmission

	Bit RXEN0(4) -> Activate the reception UART
		- RXEN0 = 0 -> Disable reception
		- RXEN0 = 1 -> Activate reception
	
	- Bit UCSZ02(2) -> Char size
		- Used with UCSZ01 and UCSZ00 to configure the data size

	- Bit RXCIE0(7) -> RX Complete Interupt Enable
		- RXCIE0 = 0 -> Disable the Reception of Interupt 
		- RXCIE0 = 1 -> Activate the Reception of Interupt

UCSR0C -> UART Control and Status Register C (Configurate the format of data)
	- Used to configurate the UART data format (8N1)
		- 8 -> 8 bit for each data
		- N -> No parity
		- 1 -> One stop bit
	
	-Bits UMSEL01(7), UMSEL00(6) -> USART Mode Select
		- 00 -> Asynchronous USART Mode
			-> Transmit and receives data without a shared clock signal
		// here Asynchronous USART Mode page 161 datasheet

	- Bits UCSZ01(2), UCSZ00(1) -> Char size
		- Define the number of data bits in each UART frame
		- WOrk with UCSZ02 in UCSR0B
		- For 8 bit data -> UCSZ02 = 0, UCSZ01 = 1, UCSZ00 = 1
		- 8 bit data being 1 char

	- Bits UPM01(5), UPM00(4) -> Parity Mode
		-> Optional bit used for simple transmittion error detection
		-> 00 -> No parity
			-> disable parity
		For 8N1 -> 00

	- Bit USBS0(3) -> Stop Bit Select
		-> Stop bit -> Marks the end of a UART frame
		-> UsBs0 = 0 -> One stop bit
		-> USBS0 = 1 -> Two stop bit
		For 8N1 -> 0

UDR0 -> USART 1/0 Data Register
	- Register used to send or received UART data
	- Transmission -> Write a char into UDR0
	- Reception -> Read a received char from UDR0
	Like a buffer kinda

ISR(USART_RX_vect) -> Interupt Service Routine
	-> ISR=> automatic fonction executer when a interuption occurs
	-> USART_RX_vect -> Interupt trigger when a new char is received through UART
		-> the received char is in UDR0
	Datasheet page 50
sei() -> to autorise the global interuption

// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //

// Calculate the UART baud rate (speed of transmission) page 146 datasheet
	- Atmega328P runs at 16Mz, but UART needs to trasnmit at 115200 baud
	- The UART clock must be divided to get the desired transmission speed
	- We compare the normal and double speed to find the smallest baud rate error

- Formula to calculate UBRR0 -> F_CPU / (16 * UART_BAUDRATE) - 1
-> Formula to calculate the actual baud rate -> F_CPU / (16 * (UBRR0 + 1))

	- Normal asynchronous mode (U2X00 = 0)
		UBRR0 = 16 000 000 / (16 * 115200) -1 = 7.68
			-> UBRR0 takes only integer so 7.68 = 8
		Actual Baud rate = 16 000 000 / (16 * (8 + 1)) = 111111 baud

	- Asynchronous double speed mode (U2X0 = 1) -> le 16 devient 8
		UBRR0 = 16 000 000 / (8 * 115200) -1 = 16.38
			-> UBRR0 takes only integer so 16.38 = 16
		Actual Baud rate = 16 000 000 / (8 * (16 + 1)) = 117647 baud
*/