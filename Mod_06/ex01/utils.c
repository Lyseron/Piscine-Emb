// To know the specific name of the components AVR in the right chip (here atmega328p)
#include <avr/io.h>
// For the _delay_ms
#include <util/delay.h>
#include <stdbool.h>
#include <util/twi.h>

// ------------------------------------------- DEFINE & VARIABLE ---------------------------------------------- //

// Datasheet page -> 
#define AHT20 0x38

void uart_printstr(const char *str);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
void i2c_read(void);

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Send the AHT20 address in WRITE or READ mode
// mode == 1 -> read
// mode == 0 -> write
// Return true if ACK was received, false otherwise
bool send_address(uint8_t mode)
{
	// Start read communication
	i2c_start();

	// Start receiving in read or write mode
	// reading mode -> get the result of the mesure
	TWDR = (AHT20 << 1) | mode;

	// Start transmitting the address
	// -> TWIN = 1 -> clear flag and start the operation
	// -> TWEN = 1 -> keep TWI enabled
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait until the address transmission is completed
	while (!(TWCR & (1 << TWINT)))
	{}

	// TW_MT -> adress send in write, ACK received
	// TW_MR -> adress receive in read, ACK received
	if (mode == 0)
		return (TW_STATUS == TW_MT_SLA_ACK);
	else
		return (TW_STATUS == TW_MR_SLA_ACK);
}

// get the first byte of the data received from AHT20 -> Status byte
bool getStatus(uint8_t *status)
{
	// Start communication in READ mode
	// -> Return true if the sensor acknoledges its address
	if (send_address(1) == false)
	{
		i2c_stop();
		return (false);
	}
	// Receive one Status byte with NACK
	// -> TWINT = 1 -> start reception
	// -> TWEN = 1 -> keep TWI enabled
	// -> TWEA = 0 -> send NACK because only one byte is requested here
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait until the status byte is received
	while (!(TWCR & (1 << TWINT)))
	{}

	// -> TW_MR_DATA_NACK = 0x58 -> byte received and NACK returned by the master
	if (TW_STATUS != TW_MR_DATA_NACK)
	{
		i2c_stop();
		return (false);
	}
	*status = TWDR;
	
	// Send stop -> End the READ communication
	i2c_stop();
	return (true);
}

// Start the measurment of the ATH20
bool start_measurement(void)
{
	// Send START and AHT20 address in WRITE mode
	// -> Write = 0 -> Master wants to send a command to the sensor
	// -> Return true if the sensor acknowledges its adress
	if (send_address(0) == false)
	{
		i2c_stop();
		return (false);
	}
	// Send the command to trigger a mesurement -> AHT20 datasheet -> page 8
	// -> 0xAC = mesurement command
	// -> 0x33 = first parameter
	// -> 0x00 = second parameter
	i2c_write(0xAC);
	i2c_write(0x33);
	i2c_write(0x00);

	// Send Stop -> end the WRITE communication
	i2c_stop();

	return (true);
}

// Convert a 8 bit value into a string containing 2 hexadecimal digits
void convert_decimal_to_hex(uint8_t value, char result[3])
{
	uint8_t first = value / 16;
	uint8_t second = value % 16;
	const char letters[] = "ABCDEF";

	if (first <= 9)
		result[0] = first + '0';
	else
		result[0] = letters[first - 10];

	if (second <= 9)
		result[1] = second + '0';
	else
		result[1] = letters[second - 10];
	result[2] = '\0';
}

// Print one byte as two hexadecimal characters on the PC
void print_hex_value(char c)
{
	// Create a string containing 3 char -> two hex and '\0'
	char result[3];

	// Convert the byte to hex
	convert_decimal_to_hex((uint8_t)c, result);

	// Print it using UART
	uart_printstr(result);
}

// Check if the AHT20 is calibrated
// -> If bit 3 of the sensor Status = 0, send initiallization command
// -> If bit 3 = 1, calibration is already enabled
void check_status_and_init_if_needed(void)
{
	// Wait after power on before communicating with the sensor
	_delay_ms(40);

	// Stock the first octet (status) of the value send by ath20 
	uint8_t status;

	// Read the sensor status
	if (getStatus(&status) == false)
	{
		uart_printstr("AHT20 address not acknoledged, failed\r\n");
		return ;
	}

	// Check calibration enable bit 3 from step 1 datasheet page 8
	if (!(status & (1 << 3)))
	{
		// Start a new communication in WRITE mode
		// -> Send the sensor address and check ACK
		if (send_address(0) == true)
		{
			// Send the command to trigger a mesurement -> AHT20 datasheet -> page 8
			// -> 0xBE = mesurement command
			// -> 0x08 = first parameter
			// -> 0x00 = second parameter
			i2c_write(0xbe);
			i2c_write(0x08);
			i2c_write(0x00);
		}
		else
			uart_printstr("Failed calibration\r\n");

		// End the WRITE communication
		i2c_stop();

		// Wait for the sensor initialization
		_delay_ms(10);
	}
}
