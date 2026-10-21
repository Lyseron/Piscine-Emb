// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// for the _delay_ms
#include <util/delay.h>

// ------------------------------------------- DEFINE & VARIABLE ---------------------------------------------- //

uint8_t value = 0;
char	hex_value[3];

// ------------------------------------------- FUNCTION DECLARATION ------------------------------------------- //

void adc_init(void);
void init_rgb(void);

void led_init(void);
void led_jauge(uint8_t value);
void wheel(uint8_t pos);

// ------------------------------------------- MAIN FUNCTION -------------------------------------------------- //

// Ex04 -> Read the value of the potentiometre and display it as a jauge with D1-D4 LED and as a color with RGB LED
int main(void)
{

	// Init the potensiometre
	adc_init();

	// Init the Leds
	led_init();

	// Init the RGB led
	init_rgb();

	// Infinite loop
	while (1)
	{
		// Start the conversion
		ADCSRA |= (1 << ADSC);

		// Wait the end of the conversion -> during the conversion ADSC = 1 when done ADSC = 0)
		while (ADCSRA & (1 << ADSC))
		{}

		// Tell if the conversion is done
		if (ADCSRA & (1 << ADIF))
			// Stock the result of the conversion
			value = ADCH;

		// Turn on different led depending on the value read from the potentiometre
		led_jauge(value);
		
		// Turn on different color depending on the value read from the potentiometre
		wheel(value);

		// Wait a bit
		_delay_ms(20);
		
	}
}

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

void adc_init(void)
{
	// define the tension
	ADMUX = (1 << REFS0);
	ADMUX &= ~(1 << REFS1);

	// To align the result to the left so we can get the 8 large bit in adch
	ADMUX |= (1 << ADLAR);

	// Select the ADC0 -> potentiometre
	ADMUX &= ~((1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3));

	// Enable ADC
	ADCSRA = (1 << ADEN);

	// Set a prescaler of 128 to get 125 000 kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
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

// -------------------------------- ADC ----------------------------- //

// ------------------ ADC ------------------ //

ADC -> Analog to Digital Converter 10 bit
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
			-> REFS1 = 0, REFS0 = 1 -> AVCC (analog supplly voltage)
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
	ADCL -> |  b7 |  b6 |  b6 |  b4 |  b3 |  b2 |  b1  |  b0  |
		-> To read the full 10 bit result:
			-> Read ADCL first
			-> Then read ADCH
When ADLAR = 1:
	ADCH -> |  b9 |  b8 |  b7 |  b6 |  b5 |  b4 |  b3  |  b2  |
	ADCL -> |  b1 |  b0 |  0  |  0  |  0  |  0  |  0   |  0   |
		-> Only ADCH is required for 8 bit precision
		-> ADCH contains the 8 most significant ADC bits, the last two are discarded
	Datasheet page -> 206, 2219-220

// ------------------ DIDR0 ------------------ //

DIDR0 -> Digital Input Disable Register 0
	-> DIsable digital input buffer on analog input pins
		-> Can reduce power consumption when using ADC
	-> Writting 1 disable the corresponding digital input buffer
		DIDR0 |= (1 << ADC0D) -> or other ADCnbD
	Datasheet page -> 220

*/