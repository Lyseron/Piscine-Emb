# -------------------------------------- Chip --------------------------------------- #

MCU        = atmega328p
BOARD      = arduino
PORT_LINUX	   = /dev/ttyUSB0
PORT_MAC   = /dev/cu.usbserial-120

CPU_SPEED  = 16000000UL
BAUD_RATE  = 115200

# -------------------------------------- Tools -------------------------------------- #

AVR_DUDE   = avrdude
OBJ_CPY    = avr-objcopy

# -------------------------------------- Compiling ---------------------------------- #

CC         = avr-gcc
CFLAGS     = -Os -Wall -Wextra -Werror -mmcu=$(MCU) \
	-DF_CPU=$(CPU_SPEED) -DUART_BAUDRATE=$(BAUD_RATE)


# -------------------------------------- OS ----------------------------------------- #

UNAME_S    := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	PORT = $(PORT_MAC)
else ifeq ($(UNAME_S),Linux)
	PORT = $(PORT_LINUX)
endif

# -------------------------------------- Files -------------------------------------- #

SRCS       = main
BUILD_DIR  = build

BIN        = $(BUILD_DIR)/$(SRCS).bin
HEX        = $(BUILD_DIR)/$(SRCS).hex

# -------------------------------------- Rules -------------------------------------- #

all : flash

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

hex : $(SRCS).c | $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $(BIN) $(SRCS).c
	@$(OBJ_CPY) -O ihex $(BIN) $(HEX)

flash : hex
	$(AVR_DUDE) -p $(MCU) -c $(BOARD) -P $(PORT) \
		-b $(BAUD_RATE) -U flash:w:$(HEX):i
	@printf "🩷\033[35m  All Good \033[0m🩷\n"

clean:
	@rm -rf $(BUILD_DIR)
	@printf "\033[32m.Bin & .Hex cleaned\033[0m\n"
fclean: clean
	@printf "\033[95mFull clean\033[0m\n"

re: fclean all

# ----------- Extra -------------- #

cputil: hex
	avr-objdump -d -S $(BIN) | less
uart :
	screen $(PORT) $(BAUD_RATE)
# screen -ls -> to check the sessions screen
# screen -r NB -> get the NB session -> to get the session back
# kill NB -> to close the session
# to close proprely a session -> Ctrl + a -> k -> y


.PHONY: all clean fclean flash hex re cputil