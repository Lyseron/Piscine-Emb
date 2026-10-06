# -------------------------------------- Chip --------------------------------------- #

MCU        = atmega328p
BOARD      = arduino
PORT	   = /dev/ttyUSB0

CPU_SPEED  = 16000000UL
BAUD_RATE  = 115200

# -------------------------------------- Tools -------------------------------------- #

AVR_DUDE   = avrdude
OBJ_CPY    = avr-objcopy

# -------------------------------------- Compiling ---------------------------------- #

CC         = avr-gcc
CFLAGS     = -Os -Wall -Wextra -Werror -mmcu=$(MCU) \
	-DF_CPU=$(CPU_SPEED) -DUART_BAUDRATE=$(BAUD_RATE)

# -------------------------------------- Files -------------------------------------- #

SRCS       = main

# -------------------------------------- Rules -------------------------------------- #

all : hex flash

hex : $(SRCS).c
	$(CC) $(CFLAGS) -o $(SRCS).bin $(SRCS).c
	$(OBJ_CPY) -O ihex $(SRCS).bin $(SRCS).hex 
flash : hex
	$(AVR_DUDE) -p $(MCU) -c $(BOARD) -P $(PORT) \
		-b $(BAUD_RATE) -U flash:w:$(SRCS).hex:i

clean:
	rm -rf *.bin *.hex
fclean: clean
re: fclean all

.PHONY: all clean fclean flash hex re