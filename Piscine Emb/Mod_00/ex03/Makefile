CC         = avr-gcc
# avr-gcc -> 

MCU        = atmega328p
# -mmcu=atmega328p ->

CPU_SPEED  = 16000000UL
# -DF_CPU ->

AVR_DUDE   = avrdude
#avrdude -> 

BOARD      = arduino
# arduino -> 

OBJ_CPY    = avr-objcopy
# avr-objcopy -> 

BAUD_RATE  = 115200
# Baud rate -> 

PORT	   = /dev/ttyUSB0
# Port -> 

CFLAGS     = -Os -Wall -Wextra -Werror -mmcu=$(MCU) -DF_CPU=$(CPU_SPEED) -DUART_BAUDRATE=$(BAUD_RATE)
# -Os ->

SRCS       = main
RM         = rm -rf

all : $(SRCS).hex flash

$(SRCS).hex: $(SRCS).bin
	$(OBJ_CPY) -O ihex $< $(SRCS).hex 
# -O ->
# ihex ->

$(SRCS).bin : $(SRCS).c
	$(CC) $(CFLAGS) -o $@ $<
# -s ->
# -o ->

flash : $(SRCS).hex
	$(AVR_DUDE) -p $(MCU) -c $(BOARD) -P $(PORT) -b $(BAUD_RATE) -U flash:w:$(SRCS).hex:i
# -p ->
# -P ->
# -b ->
# -U ->
# flash:w ->

clean:
	$(RM) *.bin *.hex

fclean: clean

re: fclean all

.PHONY: all clean fclean flash