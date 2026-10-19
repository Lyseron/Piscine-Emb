// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>

#include <util/twi.h>

// ------------------------------------------- DEFINE & VARIABLE ---------------------------------------------- //

// Datasheet page -> 
#define AHT20 0x38

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void uart_init(void);
void uart_printstr(const char *str);

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
void i2c_read(void);
void print_hex_value(char c);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

// Ex00 -> 
int main(void)
{
	// Init the UART
	uart_init();

	// Init the I2C at 100kHz
	i2c_init();

	// Infinite loop
	while (1)
	{
		// Send a START on the I2C bus -> start the comuunication -> send to all peripherique that a transmission IC2 as started
		i2c_start();

		
		// Send a STOP on the I2C bus -> Stop the communication
		i2c_stop();
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
	// Used to request master control of the bus
	TWCR |= (1 << TWSTA);

	// Keeps TWI hardware to control SDA and SCL Enable and remove the last operation flag and alllow the TWI to continue with the next operation
	TWCR |= (1 << TWEN) | (1 << TWINT);

	// Wait for the end of the operation -> = 1 = is finish
	while (!(TWCR & (1 << TWINT)))
	{}

	// Check if the START condition was transmited succefully
	//	TW_STATUS -> Store the status of TWSR -> resuslt of operation
	//	TW_START -> 0x08 -> START confition has be transmitted
	// Datahseet tab for code page -> 186
	if (TW_STATUS == TW_START)
		uart_printstr("START transmisson successfull STATUS CODE: 0x08\r\n");
	else
		uart_printstr("START transmisson failed\r\n");

}

// Send a STOP on the I2C bus -> Stop the communication
void i2c_stop(void)
{
	// To terminate the master communication -> generate a stop condition
	TWCR |= (1 << TWSTO);

	// Keeps TWI hardware to control SDA and SCL Enable and remove the last operation flag and alllow the TWI to continue with the next operation
	TWCR |= (1 << TWEN) | (1 << TWINT);

	// Wait for the end of the operation -> = 0 = stop is executed -> TWSTO is automaticly clear by hardware (0)
	while (TWCR & (1 << TWSTO))
	{}

	// print the success
	uart_printstr("STOP transmisson completed\r\n");

}

void i2c_write(unsigned char data)
{
	// Load the data into TWDR
	TWDR = data;

	// STart transmitting the data
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait until transmission is completed
	while (!(TWCR & (1 << TWINT)))
	{}

	// CHeck if the data was acknowledged
	if (TW_STATUS == TW_MT_DATA_ACK)
		uart_printstr("DATA acknowledged, success\r\n");
	else
		uart_printstr("DATA not acknowledged, failed\r\n");
		
}

void i2c_read(void)
{

}

void print_hex_value(char c)
{

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

// -------------------------------- PROTOCOL ----------------------------- //

// ------------------ I2C ------------------- //

I2C -> Inter-Integrated Circuit -> The protocol for the hardware TWI
	-> Serial communication protol that allows communication between multiple device
	-> Used only 2 communication lines
	-> SCL -> Serial Clock line
		-> Carries the clock signal
		-> Synchronizes data transmission
		-> Usually generated by master

	-> SDA --> Serial Data Line
		-> Carries data between devices
		-> Biderectional communication

	-> Master -> Control the communication
		-> Generates the clock signal
		-> Starts and stops transmission
		-> Selects the slave using its address

	-> Slave -> Respond to the master
		-> Receives or sends data when addressed
	
	-> START -> Begins an I2C transmission
	-> STOP -> Ends a I2C transmission
	-> ACK -> ACKnozledge, confirms byte reception
	-> NACK -> Not ACKnowledge, no acknolegment received
	-> Each data byte contains 8 bit
	-> An ACK/NACK bit follows each byte
		->
	
	-> Support multiple device on the same bus
		-> bus ->
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
			-> Used to wait for operation completion

		-> Bit TWEA(6) -> TWI Enable Acknowledge
			-> Control ACK generation
			-> = 1 -> Enable automatic ACK generation
			-> = 0 -> Disable automatic ACK generation
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