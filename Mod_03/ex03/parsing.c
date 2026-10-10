// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// to use boolean
#include <stdbool.h>

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

// Check if the char is between 0 and 9 or A and F
bool is_good_value(char c)
{
	if (c >= '0' && c <= '9')
		return (true);
	else if (c >= 'A' && c <= 'F')
		return (true);
	else
		return (false);
}

// Return the size of a string
uint8_t size(const char *str)
{
	uint8_t i = 0;
	while (str[i])
		i++;
	return (i);
}

// Return false if the string is not a valid input
bool parse_input(const char *str)
{
	if (str[0] != '#')
		return (false);
	if (size(str) != 7)
		return (false);
	for (uint8_t i = 1; i < 7; i++)
		if (is_good_value(str[i]) == false)
			return (false);
	return (true);
}

// Convert a char into a nb or if i'ts a letter formula to convert the ascii to decimal (base 10)
uint8_t hex_value(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else
		return (c - 'A' + 10);
}

// Convert an hex value to a decimal one
uint8_t convert_hex_to_decimal(char first_nb, char second_nb)
{
	return (hex_value((first_nb)) * 16 + hex_value((second_nb)));
}

// Fill the decimal value of each color
void get_rgb(uint8_t *r, uint8_t *g, uint8_t *b, const char *str)
{
	*r = convert_hex_to_decimal(str[1], str[2]);
	*g = convert_hex_to_decimal(str[3], str[4]);
	*b = convert_hex_to_decimal(str[5], str[6]);
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //
/*
A color is correct if it has this format -> #RRGGBB
	To be valid the input must ->
		- Start with a '#'
		- Be 7 char long
		- Be a nb between 0 and 9 or be a letter in cap between A and F

// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //

Formula to a hex value to decimal -> firstNb * 16 + secondNb
	Exemple : #FF6672
		FF -> RED -> F = 15, F = 15 (15 * 16 + 15) -> RED = 255
		66 -> GREEN -> (6 * 16 + 6) -> GREEN = 102
		72 -> BLUE -> (7 * 16 + 2) -> BLUE = 114
*/