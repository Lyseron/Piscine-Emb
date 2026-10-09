// to know the specific name of the components AVR in the right microcontroller (here atmega328p in makefile)
#include <avr/io.h>
// to use boolean
#include <stdbool.h>

// ------------------------------------------- FUNCTION UTILS ------------------------------------------------- //

bool is_good_value(char c)
{
	if (c >= '0' && c <= '9')
		return (true);
	else if (c >= 'A' && c <= 'F')
		return (true);
	else
		return (false);
}

uint8_t size(const char *str)
{
	uint8_t i = 0;
	while (str[i])
		i++;
	return (i);
}

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

uint8_t hex_value(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else
		return (c - 'A' + 10);
}

uint8_t convert_hex_to_decimal(char first_nb, char second_nb)
{
	return (hex_value((first_nb)) * 16 + hex_value((second_nb)));
}

void get_rgb(uint8_t *r, uint8_t *g, uint8_t *b, const char *str)
{
	*r = convert_hex_to_decimal(str[1], str[2]);
	*g = convert_hex_to_decimal(str[3], str[4]);
	*b = convert_hex_to_decimal(str[5], str[6]);
}

// ------------------------------------------- NOTES ---------------------------------------------------------- //

// ------------------------------------------- CALCUL & FORMULA ----------------------------------------------- //