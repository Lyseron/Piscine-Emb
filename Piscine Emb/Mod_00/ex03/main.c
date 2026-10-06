// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
#include <stdbool.h>
// for the _delay_ms
#include <util/delay.h>

// true if the button is pressed
bool isPressed(int value)
{
	if (value == 0)
		return (true);
	else
		return (false);
}

// check the state twice to avoid rebound effect*
void readButton(bool *buttonPressed)
{
// to memorises the state of the button, at first = release
	bool beforeDelay = false;
	bool afterDelay = false;

	beforeDelay = isPressed((PIND & (1 << PD2)));
	_delay_ms(50);
	afterDelay = isPressed((PIND & (1 << PD2)));

	// compare the states if they are the same we take the new value otherwize the old value stays til the next loop
	if (beforeDelay == afterDelay)
		*buttonPressed = beforeDelay;
}

void checkButtonState(bool *turnOnTheLed, bool buttonPressed, bool buttonPreviouslyPressed)
{
// check the state of the button 
		// if on now and previously not on
	if (buttonPressed == true && buttonPreviouslyPressed == false)
	// toggle the light
		*turnOnTheLed = !(*turnOnTheLed);
}

// turn the led on or off
void turnLedOnOrOff(bool turnOnTheLed)
{
	if (turnOnTheLed == true)
		PORTB |= (1 << PB0);
	else
		PORTB &= ~(1 << PB0);
}

int main(void)
{
// |= changed just the concern bit to 1, so PB0 in OUTPUT (the microcontroler send a signal to the component)
	DDRB |= (1 << PB0);

// &= ~ changed just the concern bit to 0, so PD2 in INPUT (the chips read from it)
	DDRD &= ~(1 << PD2);

// |= changed just the concern bit to 1, to activate the internal pullup*
	PORTD |= (1 << PD2);

bool buttonPreviouslyPressed = false;
bool buttonPressed = false;
bool turnOnTheLed = false;

// to infinitly run the program
	while (1)
	{
		// read the state of the button twice
			readButton(&buttonPressed);
		// determine if the Led needs to be on or not
			checkButtonState(&turnOnTheLed, buttonPressed, buttonPreviouslyPressed);
		// turn on / off the Led
			turnLedOnOrOff(turnOnTheLed);
		// remember the previous state for the next loop
			buttonPreviouslyPressed = buttonPressed;
	}
}

// PORT B -> groups physics of pin (from PB0 to PB7), each pin correspond to a bit and has it's own role
	// PB0 -> bit 0 in PORT B = LED D1 in schematics

// DDRX = Data Direction Register -> to defined the direction of a pin:
// DDRB = pins of PORT B
	//		- Bitn = 1 -> OUTPUT -> the pin send a signal from the microcontroller chip to an outside component (Exemple: the chip send a signal to the led)
	//		- Bitn = 0 -> INPUT (DEFAULT VALUE) -> pin get a signal from an outside components to the microcontroller chip (Exemple: a button send an info to the chip)

// PINX -> register to read a pin Numeric state (HIGH or LOW)
// PIND = pins of PORT D
	//		- not null -> HIGH (realease button)
	//		- 0 -> LOW (pressed button)

//PORTX -> to define the state of a pin (in output):
// PORTB -> pins in output of PORT B
	//		- Bitn = 1 -> HIGH -> the pin give a voltage (close to 5V or 3.3V)
	//		- Bitn = 0 -> LOW (DEFAULT VALUE) -> the pin don't get voltage (GND/ close to 0V)

// *internal pull up -> prevents the input from floating, that could cause wrong readings due to electrical perturbation (tension variation)
// when the button is release, it keeps the pin HIGH, at a volatage close to 5V
// when pressed, the button connests the pin to GND, making it low;

// *rebound effect -> when a button's contact touches and separate rapidely as the button is pressed or release creating
// multiple changes between HIGH and LOW since it can be interpretated as multiple button presses, causes the reading to be inacurate during that time (contact bounce).
// So reading the state, waiting a bit, then reading it again and comparing both value helps avoid accepting temporary changes cause by bouncing.

// Page 58-59-60 datasheet
