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

void convert_float_to_str_humidity(float value, char str_value[6])
{
	// Datasheet ATH20 page -> 2
	int entier = (int)value;

	// Multiply by 10 to get one decimal digit
	float decimal = (value - entier) * 10.0f;

	// Round the decimal part to the nearest interger
	int decimal_entier = (int)(decimal + 0.5f);

	// If decimal rounding reaches 10
	// -> Increase the integer part and reset the decimals
	if (decimal_entier == 10)
	{
		entier++;
		decimal_entier = 0;
	}

	uint8_t cent = (entier / 100) % 10;
	uint8_t diz = (entier / 10) % 10;
	uint8_t unit = entier % 10;

	uint8_t i = 0;
	if (cent != 0)
	{
		str_value[i] = cent + '0';
		i++;
	}
	if (cent != 0 || diz != 0)
	{
		str_value[i] = diz + '0';
		i++;
	}
	str_value[i++] = unit + '0';
	str_value[i++] = '.';
	str_value[i++] = decimal_entier + '0';
	str_value[i] = '\0';
}

void convert_float_to_str_temp(float value, char str_value[8])
{
	// Datasheet ATH20 page -> 2
	uint8_t negatif = 0;
	if (value < 0)
	{
		value *= -1;
		negatif = 1;
	}
	
	int entier = (int)value;
	// Multiply by 100 to get two decimal digit
	float decimal = (value - entier) * 100.0f;

	// Round the decimal part to the nearest interger
	int decimal_entier = (int)(decimal + 0.5f);

	// If decimal rounding reaches 100
	// -> Increase the integer part and reset the decimals
	if (decimal_entier == 100)
	{
		entier++;
		decimal_entier = 0;
	}
	uint8_t cent = (entier / 100) % 10;
	uint8_t diz = (entier / 10) % 10;
	uint8_t unit = entier % 10;

	uint8_t first_decimal = (decimal_entier / 10) % 10;
	uint8_t second_decimal = decimal_entier % 10;

	uint8_t i = 0;
	if (negatif == 1)
	{
		str_value[i] = '-';
		i++;
	}
	if (cent != 0)
	{
		str_value[i] = cent + '0';
		i++;
	}
	if (cent != 0 || diz != 0)
	{
		str_value[i] = diz + '0';
		i++;
	}
	str_value[i++] = unit + '0';
	str_value[i++] = '.';
	str_value[i++] = first_decimal + '0';
	str_value[i++] = second_decimal + '0';
	str_value[i] = '\0';
}

void print_result(float temp_f, float humidity_f)
{
	char temp[8];
	char humidity[6];
	convert_float_to_str_temp(temp_f, temp);
	convert_float_to_str_humidity(humidity_f, humidity);
	uart_printstr("Temperature: ");
	uart_printstr(temp);
	uart_printstr("°C, ");
	uart_printstr("Humidity: ");
	uart_printstr(humidity);
	uart_printstr("%\r\n");
}

float get_one_temp_mesure_in_cel(uint8_t *reading)
{
	// reading[3] -> last 4 bit -> temp data
	// reading[4] -> 8 bit temp data
	// reading[5] -> 8 bit temp data

	// Store the humidity value (20 bits)
	uint32_t temp = 0;

	// get the 20 bit from the reading
	// 15 to get a mask for the first 4 bit (0000 1111)
	temp |= ((uint32_t)reading[3] & 15) << 16;
	temp |= ((uint32_t)reading[4] << 8);
	temp |= (reading[5]);
	

	// Use the formula page 9 of datasheet to convert to %
	// 2 puissance 20 = 1048576.0f
	float celcius_temp = (temp / 1048576.0f) * 200 - 50;
	return (celcius_temp);
}
float get_one_humidity_mesure_in_pourcent(uint8_t reading[7])
{
	// reading[1] -> 8 bit humidity data
	// reading[2] -> 8 bit humidity data
	// reading[3] -> first 4 bit -> humidity data

	// Store the humidity value (20 bits)
	uint32_t humidity = 0;

	// get the 20 bit from the reading
	humidity |= ((uint32_t)reading[1] << 12);
	humidity |= ((uint32_t)reading[2] << 4);
	humidity |= ((uint32_t)reading[3] >> 4);

	// Use the formula page 9 of datasheet to convert to %
	// 2 puissance 20 = 1048576.0f
	float pourcent_humidity = (humidity / 1048576.0f) * 100;
	return (pourcent_humidity);
}

float get_moyen_3mesure(float mes1, float mes2, float mes3, float nb_of_mesure)
{
	float moyenne = 0;
	if (nb_of_mesure == 1)
		moyenne = mes1 / 1;
	else if (nb_of_mesure == 2)
		moyenne = (mes1 + mes2) / 2;
	else if (nb_of_mesure == 3)
		moyenne = (mes1 + mes2 + mes3) / 3;
	return (moyenne);
}

