CC         = avr-gcc
# avr-gcc -> GCC compileur for microcontroleurs AVR
# AVR -> microcontroller archi used by atmega328p

MCU        = atmega328p
# MCU -> Microcontroller Unit: chip with a CPU, memory.
# -mmcu=atmega328p -> tells the compiler the target chip for compilation and linking.

CPU_SPEED  = 16000000UL
# CPU frequency -> to indicate the number of processor clock cycles per second, in herts.
# processor clock cycles -> to coordonate the action inside the chip, each cycle give ti es to the electrique signal to stabilise before continuating to the next
# it needs to be the same as the chip
# UL -> Unsinged long interger.

AVR_DUDE   = avrdude
# avrdude -> tool used to upload the compiled program to the microcontroller flash memory
# flash memory -> memorie that keeps the program without power (where my code stays event if the chip is unploaged).

BOARD      = arduino
# arduino -> this variable selects the progamming method of communication with the bootloader.
# bootloader -> small program already on chip that can receive and install a new "progame" (my code)

OBJ_CPY    = avr-objcopy
# avr-objcopy -> tool used to convert the compiled file into another format (here .bin to .hex)

BAUD_RATE  = 115200
# Baud rate -> communication speed per second between the computer and the board, it must match the bootloader configuration

PORT	   = /dev/ttyUSB0
# Port -> port where the arduino board is connected

CFLAGS     = -Os -Wall -Wextra -Werror -mmcu=$(MCU) -DF_CPU=$(CPU_SPEED) -DUART_BAUDRATE=$(BAUD_RATE)
# -Os -> optimizes the program to reduce its size
# -mmcu=$(MCU) -> select the chip atmega328p
# -DF_CPU=$(CPU_SPEED) -> tells the CPU speed
# -D -> define a macro

SRCS       = main
RM         = rm -rf

all : hex flash

hex : $(SRCS).c
	$(CC) $(CFLAGS) -o $(SRCS).bin $(SRCS).c
	$(OBJ_CPY) -O ihex $(SRCS).bin $(SRCS).hex 
# -O -> select the output file format
# ihex -> Intel HEX: text records containing data and addresses 

flash : hex
	$(AVR_DUDE) -p $(MCU) -c $(BOARD) -P $(PORT) -b $(BAUD_RATE) -U flash:w:$(SRCS).hex:i
# -p -> selects the AVR microcontroller model (atmega328p)
# -c -> selects the communication method (arduino)
# -P -> selects the seial device path
# -b ->select the serial communication baud rate
# -U -> request a memory operation , memory:operation:filename:format.
# flash -> program flash memory
# w -> write: cpy the file's program data unto the selected memeory
# i -> tells avrdude that the input file uses Intel Hex format.

clean:
	$(RM) *.bin *.hex

fclean: clean

re: fclean all

.PHONY: all clean fclean flash hex re