// To know the specific name of the components AVR in the right chip (here atmega328p)
#include <avr/io.h>
// For the _delay_ms
#include <util/delay.h>
#include <avr/eeprom.h>

// ------------------------------------------- DEFINE & VARIABLE ---------------------------------------------- //



// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void init_eeprom(void);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

// Exo 00 -> 
int main(void)
{
	init_eeprom();

	while (1)
	{
		for (uint16_t i = 0; i < 1024; i++)
		{
			eeprom_read_byte(i);
			
		}
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

void init_eeprom(void)
{

}

// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*

Expected Result ->

0000 0010      57 68 61 74 20 64 6f 20 79 6f 75 20 63 61 6c 6c        |What do you call|
 Address						Value of octet in							char ASCII
  hex								  hex

Address hex -> Adress of the first octet 
57 68 61 ... -> Value of the Eprom octet in hex
[....] -> ACSII of the oectet (.) -> non printable ascii

EEPROM -> Electrically Erasable Programmable Read-Only Memore
	-> We can read and write in that memory
	-> the data send in it are saved even when the chip is not plugged
	-> Contains 1024 bytes
	-> Each byte has 8 bit (value between 0 and 255 or in hex -> 0x00 and 0xFF)
	-> It is organized like a suite of memory cases

		Adresse                 | Value in it
  0->	0x000					|  32
		0x001					|  1
		...
 1024->	0x3FF					|  12

	-> To select a case from the 1024 ones we need a 10 bit address
With the atmega there is 4 register to configurate the eeprom:
	-> EEAR -> Choose the addres of the octet to read
	-> EEDR -> Get the value of the octet
	-> EECR -> Start the operation of reading the value
	-> EEARH / EEARL -> two 8 bit register to stock EEAR (address of the octet)

	EEARH + EEARL: To choose the address --> EECR: start the reading at the chosen address -> EEDR contains the value at the chosen octet 

// -------------------------------- PROTOCOL ----------------------------- //

// ------------------ EEARL ------------------- //

EEARL -> EEPROM Address Register Low (8 bit)
	-> Used to indicated the 8 bit low of the adress that we want to read

	-> Bit     -> |  EEAR7 |  EEAR6 |  EEAR5 |  EEAR4 |  EEAR3 |  EEAR2 |  EEAR1  |  EEAR0  |
	-> Value   -> |   128  |    64  |   32   |   16   |    8   |    4   |    2    |    1    |

	-> Only value from 0 to 255, for value above we add the EEARH

// ------------------ EEARH ------------------- //

EEARH -> EEPROM Address Register High (8 bit)
	-> Used to indicated the 8 bit high of the adress that we want to read

	-> Bit 1 -> EEAR9 -> bit 9 of the address
	-> Bit 0 -> EEAR8 -> bit 8 of the address

	high = EEARL;
	low = EEARH;
To combine -> value = (high << 8) | low;

// ------------------ EEDR -------------------- //

EEDR -> EEPROM Data Register (8 bit)
	-> To temporarely stock the octet(data) transfert from the cpu to the eeprom
	-> to receive the byte we juste read

	-> Bit     -> |  EEDR7 |  EEDR6 |  EEDR5 |  EEDR4 |  EEDR3 |  EEDR2 |  EEDR1  |  EEDR0  |
	-> Value   -> |   128  |    64  |   32   |   16   |    8   |    4   |    2    |    1    |

// ------------------ EECR -------------------- //

EECR -> EEPROM Control Register (8 bit)
-> To command and watch the operation EEPROM

	-> Bit EEPM1(5) and EEPM0(4) -> choose the mode of programation of the EEPROM
		EEPM1		EEPM0		Fonction
		0			0			Erase and write in one operation
		0			1			Erase only
		1			0			Write only
	Datasheet page 21

	-> Bit EERIE(3) -> Ready Interupt Enable
		-> To enable an interupt regarding the availability of the EEPROM
			-> = 0 -> disable
			-> = 1 -> enable if global interupt also enable

	-> Bit EEPME(2) -> Master Programming Enable
		-> To autorize a writing operation
			-> to avoid wrong modification, we have a particular sequence to start a writing
				-> EEMPE needs to be activated before EEPE

	-> Bit EEPE(1) -> EEPROM Programming Enable
		-> To start a wrinting and also know if a writing is in process
			-> = 0 -> No wrinting in process
			-> = 1 -> EEPROM busy writing
		-> We need to verifie it before a new writing

	-> Bit EERE(0) -> EEPROM Read Enable
		-> To start a reading
		-> When we put it at 1 the EEPROM read the data located at the address indicated in EEAR
		-> The data is then placed in EEDR

Datasheet page 20 - 21

// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //



*/