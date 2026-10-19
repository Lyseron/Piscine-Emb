// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>

// ------------------------------------------- DEFINE & VARIABLE ---------------------------------------------- //

uint16_t value = 0;
char cel_value[5];

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void uart_init(void);
void uart_printstr(const char *str);

void adc_init(void);
uint16_t adc_return_value(void);
int convert_decimal_to_celcius(uint16_t value);
void convert_celcus_to_str(int value, char str_value[5]);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

// Ex03 -> Read the value of the internal temp captor and display it in the console in celcius format
int main(void)
{
	// Init the UART
	uart_init();

	// Init the potensiometre
	adc_init();

	// Infinite loop
	while (1)
	{
		// Get the value from ACD
		value = adc_return_value();

		// Convert the value to Celcius then str
		int cel = convert_decimal_to_celcius(value);
		convert_celcus_to_str(cel, cel_value);

		// Print the value to the console
		uart_printstr(cel_value);
		uart_printstr("\n\r");

		// Wait 20ms to print the next value
		_delay_ms(20);
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

void adc_init(void)
{
	// define the tension to 1.1V
	ADMUX = (1 << REFS0) | (1 << REFS1);

	// To align the result to the right so we can get the 10bit value
	ADMUX &= ~(1 << ADLAR);

	// Enable ADC to convert value
	ADCSRA = (1 << ADEN);

	// Set a prescaler of 128 to get 125 000 kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_return_value(void)
{
	uint16_t low = 0;
	uint16_t high = 0;
	uint16_t value = 0;

	// Select the ADC8 -> internal temp
	ADMUX |= (1 << MUX3);

	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait the end of the conversion -> during the conversion ADSC = 1 when done ADSC = 0)
	while (ADCSRA & (1 << ADSC))
	{}

	// Tell if the conversion is done
	if (ADCSRA & (1 << ADIF))
	{
		// Stock the result of the conversion
		low = ADCL;
		high = ADCH;
		value = (high << 8) | low;
	}
	return (value);
}

int convert_decimal_to_celcius(uint16_t value)
{
	int temp;
	temp = 25 + ((value - 352) * 100) / 128;
	return (temp);
}

void convert_celcus_to_str(int value, char str_value[5])
{
	uint8_t negatif = 0;
	if (value < 0)
	{
		value *= -1;
		negatif = 1;
	}
	uint8_t cent = (value / 100) % 10;
	uint8_t diz = (value / 10) % 10;
	uint8_t unit = value % 10;

	if (negatif == 1)
		str_value[0] = '-';
	else
		str_value[0] = ' ';
	str_value[1] = cent + '0';
	str_value[2] = diz + '0';
	str_value[3] = unit + '0';
	str_value[4] = '\0';
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*
// -------------------------------- HARDWARE ------------------------- //

Potentiometre RV1 -> Variable resistor used to adjust a voltage manually
	-> By rotating the potentiometer, its resistance ratio changes
	-> Output voltage varies between GND(0V) and +5V
	-> Connected to ADC_POT
	-> ADC_POT -> PC0 / ADC0 (pin 23)
	-> ADC 10 bit value -> used to mesure the potentiometer position

LDR L14 -> Light Dependent resistor (photoresistor)
	-> Resistance changes depending on ligh intensity
		-> More light -> lower resistance
		-> Less light -> higher resistance
	-> Output signal = ADC_LDR
	-> ADC_LDR => PC1 / ADC1 (pin 24)
	-> ADC 10 bit value -> Used to mesure ambient light level
		-> More light -> higher ADC value
		-> Less light -> lower ADC value

NTC R20 -> Negative Temperature Coefficient thermistor
	-> Resistance changes depending on temperature
		-> Highter temperature -> Lower resistance
		-> Lower temperature -> Higher resistance
	-> ADC_NTC -> PC2 / ADC2 (pin 25)
	-> ADC 10 bit value -> Used to measure temperature changes
		-> Highter temperature -> Higher ADC value
		-> Lower temperature -> Lower ADC value

// -------------------------------- ADC ----------------------------- //

// ------------------ ADC ------------------ //

ADC -> Analog to Digital Converter 10 bit -> Read a voltage
	-> Convert an analog voltage into a digital value
	-> 10 bit result -> value between 0 and 1023
	-> 8 bit result -> Value between 0 and 255
	Datasheet page 205-208

// ------------------ ADMUX ------------------ //

ADMUX -> ADC Multiplexer Selection Register
	-> 8-Bit register to configure the ADC
			-> Which reference voltage to use
			-> how to organize the 10-bit result
			-> Which analog input of the microcontroller to mesure (ADC0)
	- Bit REFS1(7), REFS0(6) -> Reference selection Bits -> define the reference voltage used by the ADC
			-> REFS1 = 1, REFS0 = 1 -> Intern 1,1V -> For the interner temp captor page 215
				->
	-> Bit ADLAR(5) -> ADC Left Adjust Result
			-> define how the 10 bit result is stored in ADCH and ADCL
				ADLAR = 0 -> Result aligned to the right
					-> ADCL contains bits 7 to 0
					-> ADCH contains bits 9 and 8 (in it's two lowest position)
					-> Useful to get the complete 10 bit result
				ADLAR = 1 -> Resullt aligned to the left
					-> ADCH contains bits 9 to 2
					-> ADCLL contains bit 1 and 0 in it's two highest position
					-> Useful when we only need 8 bit precision
	-> Bit MUX3(3), MUX2(2), MUX1(1), MUX0(0) -> Multiplexer Selection Bits
		-> the MUX bit make a binary nb that determine the selected ADC
			-> For internal temp captor -> ADC8 -> bit MUX3 = 1 -> tab page 218
	Datasheet page -> 217-218

// ------------------ ADCSRA ------------------ //

ADCSRA -> ADC Control and Status Register A -> 8 bit register to controll the ADC
	-> Bit ADEN(7) -> Enable or disable ADC
	-> Bit ADSC(6) -> Start a conversion (during the conversion ADSC = 1 when done ADSC = 0)
	-> Bit ADATE(5) -> ADC Auto Trigger Enable
		 -> ADC can be trigger by a selected hardware event
		-> Trigger source configured using ADTS bits in ADCSRB
	-> Bit ADIF(4) -> ADC Interrupt flag
		-> Seti to 1 by hardware ehen a conversion is finiched
		-> Can be checked to detect the end of a conversion
		-> Clear when the ADC conversion interupt is executed and by writting 1 to ADIF
	-> Bit ADIE(3) -> ADC Interupt Enable
		-> Autorise an interupt at the end of the conversion
	-> Bit ADPS2(2) and ADPS1(1) and ADPS0(0) -> Define the divider clock of the ADC
		-> fADC = should be between 50kHz and 200kHz -> page 208 datasheet
			-> with a 128 prescaler 16 000 000 / 128 = 125 000Hz -> page 219 datasheet
				-> for 128 -> ADPS2 = 1, ADPS1 = 1, ADPS0 = 1
	Datasheet page -> 207-210, 218-219

// ------------------ ADCSRB ------------------ //

ADCSRB -> ADC Control and Status Register B -> Additional ADC configuration register
	-> Bit ACME(6) -> Analog Comparator Multiplexer Enable
		-> Can allow ADC multiplexer selection for the analog comparator
	-> Bits ADTS2(2), ADTS1(1), ADTS0(0) -> ADC Auto Trigger Source
		->Select which event can automatically start an ADC conversion
		-> Used when ADATE = 1 in ADCSRA
	Datasheet page 219-220

// ------------------ ADCL/ADCH ---------------- //

Once the conversion is done the result is in two register:
ADCL -> ADC Data Register Low
ADCH -> ADC Data Register High
	Datasheet page -> 219-220, 206 explaining

When ADLAR = 0:
	ADCH -> |  0  |  0  |  0  |  0  |  0  |  0  |  b9  |  b8  |
	ADCL -> |  b7 |  b6 |  b5 |  b4 |  b3 |  b2 |  b1  |  b0  |
		-> To read the full 10 bit result:
			-> Read ADCL first
			-> Then read ADCH
			-> Moove the 2 bit from ADCH 8 place to put them at the end 
				-> 0000 0011 -> 11 0000 0000
			-> Then place the remaining 8 bit from ADCH
				-> 11 0000 0000 -> 11 1111 1111

When ADLAR = 1:
	ADCH -> |  b9 |  b8 |  b7 |  b6 |  b5 |  b4 |  b3  |  b2  |
	ADCL -> |  b1 |  b0 |  0  |  0  |  0  |  0  |  0   |  0   |
		-> Only ADCH is required for 8 bit precision
		-> ADCH contains the 8 most significant ADC bits, the last two are discarded
	Datasheet page -> 206, 219-220

// ------------------ DIDR0 ------------------ //

DIDR0 -> Digital Input Disable Register 0
	-> DIsable digital input buffer on analog input pins
		-> Can reduce power consumption when using ADC
	-> Writting 1 disable the corresponding digital input buffer
		DIDR0 |= (1 << ADC0D) -> or other ADCnbD
	Datasheet page -> 220

// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //

Calcul of the conversion from adc value to celcius 
	-> T(Cel) = (25 + (ADC - (273 + 100 - TS_OFFSET)) * 128) / TS_GAIN
		-> ADC -> Result of the mesure of the captor in 10bit (between 0 and 1023)
		-> 373 -> 273 + 100 -> Constant used in the manufacturer's calibration formula
		-> TS_OFFSET -> Coefficient signe that correct the censor offset
		-> 128 -> Scallling factor for TS_GAIN
		-> TS_GAIN correct the temperature sensor sensitivity
		-> 25 -> Temp of reference en celcius

	-> New formula cause we don't have TS_OFFSET or TS_GAIN
		-> temp = (25 + (adc_vallue - 352) * 100) / 128
			-> 352 -> Typical result at 25 cellcius 
			-> differnece between 125degre et 25degre
			-> 25 temp of comparaison

*/