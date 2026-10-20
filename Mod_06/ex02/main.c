// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdbool.h>

// ------------------------------------------- DEFINE & VARIABLE ---------------------------------------------- //

// Count the received bytess
// -> First 6 bytes -> Send ACK
// -> Last bytes(7th) -> send NACK
uint8_t counter = 0;

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void uart_init(void);
void uart_printstr(const char *str);

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
bool i2c_read_me(uint8_t *read);
void check_status_and_init_if_needed(void);
bool send_address(uint8_t mode);
bool getStatus(uint8_t *status);
bool start_measurement(void);

float get_moyen_3mesure(float mes1, float mes2, float mes3, float nb_of_mesure);
float get_one_temp_mesure_in_cel(uint8_t reading[7]);
float get_one_humidity_mesure_in_pourcent(uint8_t reading[7]);

void print_result(float temp_f, float humidity_f);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

// Ex02 -> Print the measurement of humidity and temperature of the AHT20 in pourcentage and celcius
int main(void)
{
	// Init the UART
	uart_init();

	// Init the I2C -> communication speed = 100kHz
	i2c_init();

	// Check if the AHT20 is calibrated
	// -> If calibration bit == 0, send the initialization command
	// -> If calibration bit == 1, sensor is ready
	check_status_and_init_if_needed();

	// To stock the 3 reading of temp and humidity
	float temp[3] = {0};
	float humidity[3] = {0};

	// nb of valid mesurement stored (max 3)
	uint8_t nb_of_mesure = 0;

	// Infinite loop --> repeat mesurements continuously
	while (1)
	{
		// If the measurement started correctly
		if (start_measurement() == true)
		{
			// Wait at least 80ms dor the sensor to perform the mesurement
			_delay_ms(80);

			// Send START and AHT20 address in READ mode
			// -> Read = 1 -> Master wants to receive the measurement
			// -> Return true if the sensor acknowledges its adress
			if (send_address(1) == true)
			{
				uint8_t reading[7];
				bool read_success = true;

				// Reset the counter before each mesurement
				counter = 0;

				// Read the 7 bytes returned by the sensor
				// -> Each call to i2c_read() receives one byte
				// -> Bytes 0 to 5 are received with ACK
				// -> Bytes 6 is received with NACK
				for (uint8_t i = 0; i < 7; i++)
				{
					// Read one byte and store it into reading[]
					if (i2c_read_me(&reading[i]) == false)
					{
						uart_printstr("MESURMENT failed\r\n");
						read_success = false;
						break ;
					}
				}
				// Continue only if all 7 bytes were received okay
				if (read_success == true)
				{
					// Convert the 7 bytes into temperature and humidity
					float new_temp = get_one_temp_mesure_in_cel(reading);
					float new_humidity = get_one_humidity_mesure_in_pourcent(reading);
					
					// Fill the arrays until 3 measurements are stored
					if (nb_of_mesure < 3)
					{
						temp[nb_of_mesure] = new_temp;
						humidity[nb_of_mesure] = new_humidity;
						nb_of_mesure++;
					}
					else
					{
						temp[0] = temp[1];
						temp[1] = temp[2];

						humidity[0] = humidity[1];
						humidity[1] = humidity[2];

						// Store the newest measurement
						temp[2] = new_temp;
						humidity[2] = new_humidity;
					}
					float moy_temp = get_moyen_3mesure(temp[0], temp[1], temp[2], nb_of_mesure);
					float moy_humidity = get_moyen_3mesure(humidity[0], humidity[1], humidity[2], nb_of_mesure);
					print_result(moy_temp, moy_humidity);
				}
			}
		}
		else
			uart_printstr("AHT20 address not acknoledged, failed\r\n");
		

		// Send STOP -> Release the I2C bus -> Stop the communication
		i2c_stop();

		// Wait before starting a new measurement -> Recommanded interval 2 second
		// AHT20 datasheet -> page 5
		_delay_ms(2000);
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Init the I2C at 100kHz
void i2c_init(void)
{
	// Prescaler at 1 to get to 100kHz
	TWSR &= ~((1 << TWPS1)| (1 << TWPS0));

	// Choose 72 to make the speed of communication 100kHz
	TWBR = (1 << TWBR6) | (1 << TWBR3);

	// Enable TWI hardware to control SDA and SCL
	TWCR |= (1 << TWEN);
}

// Send a START on the I2C bus -> start the comuunication -> send to all peripherique that a transmission IC2 as started
void i2c_start(void)
{
	// TWINT = 1 -> clear the previous operation flag
	// TWEN = 1 -> keep the TWI hardware enabled
	// TWSTA = 1 -> request a START condition
	// Used to request master control of the bus
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	// Wait until the START operation is completed
	// Hardware sets TWINT = 1 when the operation is finished
	while (!(TWCR & (1 << TWINT)))
	{}

	// Check if the START condition was transmited succefully
	//	TW_STATUS -> Store the status of TWSR -> resuslt of operation
	//	TW_START -> 0x08 -> START confition has be transmitted
	// Datahseet tab for code page -> 186
	if (TW_STATUS != TW_START)
		uart_printstr("START transmisson failed\r\n");

}

// Send a STOP conditions on the I2C bus -> Stop the communication and releases the bus
void i2c_stop(void)
{
	// TWINT = 1 -> alow the next TWI operation
	// TWEN = 1 -> keeps the TWI hardware enabled
	// TWSTO = 1 -> request a STOP  condition
	// To terminate the master communication -> generate a stop condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

	// Wait for the end of the operation -> = 0 = stop is executed 
	// -> TWSTO is automaticly clear by hardware (0)
	while (TWCR & (1 << TWSTO))
	{}

	// print the success
	// uart_printstr("STOP transmisson completed\r\n");

}

// Send one byte to the AHT20 through I2C
// -> The Master transmits data and the sensor return ACK or NACK
void i2c_write(unsigned char data)
{
	// Load the bytes into the TWI data Register
	// -> TWDR contains the byte that will be transmitted
	TWDR = data;

	// TWINT = 1 -> clear the flagg and start the transmission
	// TWEN = 1 -> keeps the TWI hardware enabled
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait until the byte transmission is completed
	// -> TWINT becomes 1 when the operation is finished
	while (!(TWCR & (1 << TWINT)))
	{}

	// CHeck if the data was acknowledged by the Slave
	// TW_STATUS -> Current TWI status code
	// TW_MT_DATA_ACK -> 0x28, data transmitted and ACK received
	// ACK -> says that the octect has been receive and is ready for the next
	if (TW_STATUS != TW_MT_DATA_ACK)
		uart_printstr("DATA not acknowledged, failed\r\n");
		
}


// Receive one byte from the AHT20 through I2C
// -> The sensor sends the byte
// -> The master sends ACK or NACK after receiving it
bool i2c_read_me(uint8_t *read)
{
	// Start receiving the data and TWEA for send ACK
	// ACK -> says that the octect has been receive and is ready for the next
	// NACK -> says that the octet has been receive but don't send more
	
	// Receive the first 6 bytes with ACK
	// -> TWEA = 1 -> Master sends ACK after receiving the byte
	// -> ACK tells the sensor that the Master wants the next byte
	if (counter < 6)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	else
		// Receive the last byte (7th) with NACK
		// TWEA = 0 -> Master sends NACK
		// -> NACK tells the sensor that the Master does not want more bytes
		TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait until one byte has been received
	// -> TWINT becomes 1 when reception is completed
	while (!(TWCR & (1 << TWINT)))
	{}

	// Check if the bytes was received correctly
	// -> TW_MR_DATA_ACK = 0x50 -> byte received and ACK returned
	// -> TW_MR_DATA_NACK = 0x58 -> byte received and NACK returned
	if ((counter < 6 && TW_STATUS == TW_MR_DATA_ACK)
	|| (counter == 6 && TW_STATUS == TW_MR_DATA_NACK))
	{
		// Read the received byte from TWDR
		// -> TWDR containss the last byte received from the AHT20
		*read = TWDR;
	}
	else
	{
		// The byte reception did not return the expected status
		uart_printstr("READ failed\r\n");
		return (false);
	}

	// Increment counter for the next byte
	counter++;
	return (true);
}


// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*

// -------------------------------- HARDWARE ------------------------- //

// ------------------ TWI ------------------- //

Every time we want to send a new operation to TWI we put TWINT = 1 and TWEN = 1 in TWCR
	-> TWEN = 1 -> Maintain the TWI activated
	-> TWINT = 1 -> delete the flag of end operation and autorise the TWI to continue with the next operation

TWI -> Two Wire Interface -> the harware that can used TWI
	-> Hardware peripheral in atmega
	-> Manage serical communication using SDA and SDL
	-> Can operate in master or slave mode
	-> Automatically handles data transmission
	-> Generates START and STOP conditions
	-> Handles ACK and NACK signals
	-> Provides status codes after operations
	-> Can generate interrputs

	-> SDA -> PC4 (ADC4/SDA)
	-> SCL -> PC5 (ADC5/SCL)
	Datasheet pasge 176

// ------------------ AHT20 temp sensor ------ //

Datasheet de l'AHT20 -> https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf
page -> 8
	AHT20 -> Temperature and humidity sensor
		-> Digital sensor using I2C communictaion
		-> Mesure temperature in degree Celcius
		-> Mesure humidity percentage
		-> Acts as the slave device on the I2C bus
		-> Controlled by the atMega (master)

		-> I2C address = 0x38
		-> Temp measurement = 20 bits
		-> Humidity measurement = 20 bits

	The AHT20 send a measurement using 7 bytes -> 
		-> each byte contains 8 bits
		-> TWDR con contain only one byt at the time
		-> We must read TWDR 7 times to get the full measurement
	Exemple subject -> 0C 79 9A A6 4E 3C F2
	7x 8 bit octet for the full reading:
		Octet 0 -> OC -> Status of the aht20
		Octet 1 -> 79 -> Humidity data
		Octet 2 -> 9A -> Humidity data
		Octet 3 -> A6 -> End of Humidity data and start of temp data
		Octet 4 -> 4E -> Temp data
		Octet 5 -> 3C -> Temp data
		Octet 6 -> F2 -> CRC -> check if the data is correct
	Page 8 -> aht20 datasheet 

AHT20 Status -> One byte received from the sensor
	-> Contains info about the sensor state
		-> Bit 7 -> BUSY
			-> 1 = Sensor is busy
			-> 0 = Sensor is not busy 
		-> Bit 3 -> Calibration Enable
			-> 1 = Calibration enabled
			-> 0 = Sensor initialization required

// ------------------ TWI STATUS CODE ------- //

TW_STATUS -> Indicates the result of the last TWI operation

TW_START = 0x08
	-> START conditions transmitted

WRITE MODE (MASTER TRANSMITTER):

	TW_MT_SLA_ACK = 0x18
		-> ADDress + WRITE transmitted
		-> ACK received from the slave

	TW_MT_SLA_NACK = 0x20
		-> Address + WRITE transmitted
		-> NACK received from the Slave
	
	TW_MT_DATA_ACK = 0x28
		-> Data byte transmitted
		-> ACK received from the Slave

	TW_MT_DATA_NACK = 0x30
		-> Data byte transmitted
		-> NACK received from the Slave

READ MODE (MASTER RECEIVER):

	TW_MR_SLA_ACK = 0x40
		-> Address + READ transmitted
		-> ACK received from the Salve
	
	TW_MR_SLA_NACK = 0x48
		-> Address + READ transmitted
		-> NACK received from the Slave

	TW_MR_DATA_ACK
		-> Data byte received
		-> ACK return by the Master

	TW_MR_DATA_NACK = 0x58
		-> Data byte received
		-> NACK returned by the Master

-> MT = Master Transmitter
-> MR = Master Receiver
-> SLA = Slave Address
-> DATA = Data byte
Datasheet page -> 189

// -------------------------------- PROTOCOL ----------------------------- //

// ------------------ I2C ------------------- //

I2C -> Inter-Integrated Circuit -> The protocol for the hardware TWI
	-> Serial communication protol that allows communication between multiple device
	-> Used only 2 communication lines

	-> SCL -> Serial Clock line
		-> Carries the clock signal
		-> Synchronizes data transmission
		-> Usually generated by master

	-> SDA -> Serial Data Line
		-> Carries data between devices
		-> Biderectional communication
		-> Used to transmit addresses, commands and measurement

	-> BUS -> Shared communication wires
		-> Several devices can use the same SDA and SCL lines
		-> Each Slave is selected using its I2C address

	-> Master -> Control the communication
		-> Generates the clock signal
		-> Starts the communication
		-> Select slave using its address
		-> Selects the slave using its address
		-> Sends commands
		-> Request data

	-> Slave -> Respond to the master
		-> Recognizes its address
		-> Receives commands
		-> Sends data when addressed bu master
	
	-> START -> Begins an I2C transmission
		-> Generated by the master
		-> SDA goes from HIGH to LOW whille SCL is HIGH
		-> Tells the devices that a communication is starting
	-> STOP -> Ends a I2C transmission
		-> Generated by the master
		-> SDA goes from LOW to HIGH while SCLL is HIGH
		-> Releases the I2C bus

	ACK -> Acknowlledge
		-> Confirm that an octet was acknowledged
	NACK -> Not Acknowledge
		-> No acknoledgement of the octet
		-> Can also indicate the end of a READ communication
	Each data byte contains 8 bits
		-> ACK?NACK comes after there 8 bits, it is not stored insing TWDR

	When ehe Master transmit:
		-> ATmega sends an octet 
		-> AHT20 answer with ACK or NACK

	When the master recives:
		-> AHT20 sends an octet
		-> Atmega answer eith ACK or NACK
			-> ACK -> Byte received, send the next one
			-> NACK -> Last byte received, stop sending more bytes
	Here:
		-> Master -> Atmega
		-> Slave -> AHT20 temp sensor
		-> SDA -> PD4
		-> SCL -> PD5
		-> Communication speed = 100kHz
	Datasheet pasge 176

// -------------------------------- REGISTER ----------------------------- //

// ------------------ TWDR ------------------ //

// TWDR -> TWI Data Register 8 bit (1 byte) -> To store data
	-> Contain the byte to send or the last byte received
	-> Used to exchange data through the I2C bus
	-> Bit TWD -> |  TWD7 |  TWD6 |  TWD5 |  TWD4 |  TWD3 |  TWD2 |  TWD1  |  TWD0  |
	-> Value   -> |  128  |   64  |  32   |   16  |   8   |    4  |    2   |    1   |
	-> Place a number into the register
	Datasheet page -> 200

// ------------------ TWBR ------------------ //

TWBR -> TWI Bit Rate Register
	-> Define the speed of I2C communication
	-> Bit TWBR -> |  TWBR7 |  TWBR6 |  TWBR5 |  TWBR4 |  TWBR3 |  TWBR2 |  TWBR1  |  TWBR0  |
	-> Value    -> |   128  |   64   |   32   |   16   |    8   |    4   |    2    |    1    |
		-> All 8 bit form 1 Value (0 to 255)
		-> Add them to get 72 (see formula below)
		-> TWBR = 72 for 100 kHz with F_CPU = 16
			-> Higher TWBR value = Lower SCL frequency
			-> Lower TWBR value = Higher SCl frequency
	Datasheet page -> 198

// ------------------ TWSR ------------------ //

TWSR -> TWI Control register
	-> Contains TWI status and prescaler
	-> Used to check the result of an I2C operation

		-> Bit TWS7(7) to TWS3(3) -> TWI Status
			-> Indicate the current state of I2C communication

		-> TWPS1(1) and TWPS0(0) -> Prescaler
			-> Used to select the clock divider (see datasheet)
	Datasheet page -> 200

// ------------------ TWCR ------------------ //

TWCR -> Control the TWI peripherique
	-> tells the compilator when operation to do
	-> Used to enable and siable TWI, START and STOP conditioon, control ACK generation, detect completed TWI operation

		-> Bit TWINT(7) -> TWI Interrupt Flag
			-> Set to 1 by hardware when an operation finishes
			-> Writting 1 clears the flag
			-> learing the flag alows the next operation to continue
			-> Read 1 to know if the operation is complketed
			-> Used to wait for operation completion

		-> Bit TWEA(6) -> TWI Enable Acknowledge
			-> Control ACK generation
			-> = 1 -> Rceived data and send ACK
			-> = 0 -> received data and send NACK
			-> ACK confirms that a byte was received
			-> Used when receiving data as master or slave
			-> also used for slave address recognition

		-> Bit TWSTA(5) -> TWI START Condition
			-> Used to request master control of the bus
				-> 1 = Request a START condition
			-> HArdware checks if the I2C is available
				-> If free, hardware generate START
				-> If busy, waits for the bus to become free
			-> Must be cleared by sofware after START
			-> Used to begin a transmission
		
		-> Bit TWSTO(4) -> TWI STOP condition
			-> Used to terminate master communication
				-> 1 = Request a STOP conditions
			-> Harware genera1te1 a1 1STOP on the I2C bus
			-> Automaticly cleared after STOP is sent
			-> Releases the bus after the transmission
			-> In slave mode, used for error recovery

		-> Bit TWWC(3) -> TWI Write Collision Flag
			-> Indicates an invalid wirte to TWDR
				-> 1 = Write collision detected
			-> Occurs if TWDR is written when TWINT = 0
			-> Cleared by writting TWDR when TWINT = 1
		
		-> Bit TWEN(2) -> TWI Enable
			-> Enable or disables the TWI peripheral
				-> = 1 -> Enable TWI hardware
				-> = 0 -> Disable TWi hardware
			-> When enabled, TWI controls SDA and SCL pins
			-> Enables signal filtering on SDA and SCL
				-> SDA ->
				-> SCL ->
			-> Disabling terminates ongoing TWI operations
			-> Require for I2C communication

		-> Bit TWIE(0) -> TWI Interupt Enable
			-> Controls TWI interupt request
				-> = 1 -> Enable TWI interupt request
				-> = 0 -> Disable TWI interupt request
			-> Require sei() -> global interupt enable
	Datasheet page 199

// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //

Calcul of the speed of communication (TWBR)
	-> Formula datasheet page -> 180
		-> FCPU = 16 000 000Hz
		-> SCL wanted = 100 000Hz
	-> fSCL = FCPU / (16 + 2 * TWBR * prescaler);
		-> 16 000 000 / (16 + 2 * 72 * 1)
			SCL = 100 kHz
			TWBR = 72

	Datasheet page -> 216
*/