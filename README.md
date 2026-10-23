<div align="center">

# Embedded Systems Piscine

### Low-Level C Programming on the ATmega328P

**GPIO · Timers · PWM · UART · Interrupts · ADC · I²C · EEPROM**

*A hands-on journey through microcontroller programming, from manipulating individual pins to controlling hardware peripherals directly through registers.*

</div>

---

## Table of Contents

1. [Overview and Requirements](#1-overview-and-requirements)
2. [Building and Flashing](#2-building-and-flashing)
3. [Understanding Register Operations](#3-understanding-register-operations)
4. [Module 00 — Digital Input and Output](#4-module-00--digital-input-and-output)
5. [Module 01 — Timers and PWM](#5-module-01--timers-and-pwm)
6. [Module 02 — UART Communication](#6-module-02--uart-communication)
7. [Module 03 — RGB LEDs and Serial Commands](#7-module-03--rgb-leds-and-serial-commands)
8. [Module 04 — Interrupts](#8-module-04--interrupts)
9. [Module 05 — Analog-to-Digital Conversion](#9-module-05--analog-to-digital-conversion)
10. [Module 06 — I²C Bus and AHT20 Sensor](#10-module-06--ic-bus-and-aht20-sensor)
11. [Module 07 — EEPROM Memory](#11-module-07--eeprom-memory)
12. [Register Reference](#12-register-reference)
13. [Known Limitations and Improvements](#13-known-limitations-and-improvements)
14. [References](#14-references)

---

## 1. Overview and Requirements

This repository contains exercises from an **embedded systems programming piscine** in **C**, targeting the **ATmega328P**, an 8-bit AVR microcontroller. The goal is to work directly with hardware registers rather than relying on the Arduino framework: configuring pins, generating timed signals, communicating with a computer or sensors, responding to interrupts, and accessing non-volatile memory.

Every `Mod_XX/exYY` directory contains its own source code and `Makefile`. The exercises are independent programs, **not** a single firmware image to build all at once.

| Module | Main topics | Included exercises |
|:--|:--|:--|
| [`Mod_00`](Mod_00/) | GPIO, LEDs, push buttons, debouncing | `ex00`–`ex03` |
| [`Mod_01`](Mod_01/) | Delays, Timer1, PWM, brightness control | `ex00`–`ex03` |
| [`Mod_02`](Mod_02/) | UART, transmission, reception, interrupts | `ex00`–`ex03` |
| [`Mod_03`](Mod_03/) | RGB LEDs, color mixing, PWM, UART control | `ex00`–`ex03` |
| [`Mod_04`](Mod_04/) | External interrupts, timers, pin-change interrupts | `ex00`–`ex02` |
| [`Mod_05`](Mod_05/) | ADC, potentiometer, light and temperature sensing | `ex00`–`ex04` |
| [`Mod_06`](Mod_06/) | I²C/TWI, AHT20 temperature and humidity sensor | `ex00`–`ex02` |
| [`Mod_07`](Mod_07/) | EEPROM and memory reading | `ex00` (unfinished) |

**Target environment:** `avr-gcc`, `avr-objcopy`, `avrdude`, `make`, and an ATmega328P running at the **16 MHz** clock frequency assumed by the Makefiles. The Makefiles examined use the `arduino` programmer configuration and `/dev/ttyUSB0` as the serial device. Exact LED/button wiring depends on your board. Names such as `PB0` and `PD2` refer to **microcontroller port bits**, not Arduino digital pin numbers.

> **Important:** defining `F_CPU = 16000000UL` tells the compiler what clock speed to assume for timing calculations. It does **not** configure the physical oscillator or the microcontroller fuse bits. Confirm that the hardware really runs at 16 MHz.

## 2. Building and Flashing

Enter the directory of the exercise you want to run:

```bash
cd Mod_01/ex01
make hex       # Compile and produce an Intel HEX image
make flash     # Build and flash using avrdude
make           # Default target: HEX build + flashing, depending on the Makefile
make clean     # Remove generated files
make re        # Clean, rebuild and flash
```

Typical Makefile settings:

```makefile
MCU       = atmega328p
CPU_SPEED = 16000000UL
CC        = avr-gcc
CFLAGS    = -Os -Wall -Wextra -Werror -mmcu=$(MCU) \
            -DF_CPU=$(CPU_SPEED) -DUART_BAUDRATE=115200
```

`-mmcu` selects the AVR target; `-Os` optimizes code size; `-Wall -Wextra -Werror` enable compiler warnings and turn them into errors; `-DF_CPU` supplies the clock value used by delay routines. `avr-objcopy -O ihex` produces the flashable Intel HEX file, which `avrdude` uploads to the microcontroller.

Edit `PORT`, `BOARD`, and, if needed, `BAUD_RATE` in the relevant Makefile to match your setup. **Running plain `make` may flash your board immediately.** Use `make hex` to build without uploading. Work-in-progress exercises, especially `Mod_07/ex00`, may not compile as provided.

## 3. Understanding Register Operations

AVR peripherals are controlled by setting, clearing, and reading individual bits in hardware registers declared in `<avr/io.h>`.

```c
DDRB  |=  (1 << PB0);    // Set PB0 as an output
PORTB |=  (1 << PB0);    // Drive PB0 high
PORTB &= ~(1 << PB0);    // Drive PB0 low without affecting other bits
PORTB ^=  (1 << PB0);    // Toggle PB0
if (!(PIND & (1 << PD2))) {
    // PD2 is low: button pressed in an active-low pull-up circuit
}
```

| Expression | Meaning |
|:--|:--|
| `1 << n` | Build a bit mask with only bit `n` set |
| `register |= mask` | Set selected bits; preserve all others |
| `register &= ~mask` | Clear selected bits |
| `register ^= mask` | Toggle selected bits |
| `register & mask` | Test one or more bits |
| `register = value` | Replace the **entire register**; may affect unrelated pin/peripheral settings |

For GPIO, `DDRx` controls pin direction (`1` = output, `0` = input); `PORTx` sets an output level or enables an input pull-up; and `PINx` reads a pin's logic level. A push button connected to ground with an internal pull-up is **active-low**: released = 1, pressed = 0.

---

## 4. Module 00 — Digital Input and Output

**Goal:** configure microcontroller ports, drive an LED, and read a push button.

### `ex00` — Minimal Program Structure

[View source](Mod_00/ex00/main.c). The `main(void)` function is empty. This introduces the entry point of an AVR program but does not initialize any peripherals or produce observable output.

### `ex01` — Turn On an LED

[View source](Mod_00/ex01/main.c). Configure `PB0` as an output and hold the pin high:

```c
DDRB  |= (1 << PB0);
PORTB |= (1 << PB0);
while (1) {}
```

**Registers:** `DDRB` selects `PB0` direction; `PORTB` sets its output level. `PB0` names a **bit position**, not a separate register. Whether the LED lights up when the pin is high depends on its wiring and polarity.

### `ex02` — Control an LED with a Button

[View source](Mod_00/ex02/main.c). Configure `PD2` as an input with an internal pull-up. The LED on `PB0` lights while pressing the button connects `PD2` to ground. The main loop continuously reads `PIND`:

```c
DDRD  &= ~(1 << PD2);  // Input
PORTD |=  (1 << PD2);  // Enable pull-up
if (!(PIND & (1 << PD2)))
    PORTB |= (1 << PB0);
else
    PORTB &= ~(1 << PB0);
```

**Registers:** `DDRD`, `PORTD`, `PIND` for the button; `DDRB`, `PORTB` for the LED. The pull-up prevents a floating input, whose state could otherwise be affected by electrical noise.

### `ex03` — Toggle an LED with Button Debouncing

[View source](Mod_00/ex03/main.c). Each **new button press** toggles the stored LED state. The LED stays on after the button is released, then turns off on the next press. The firmware compares two readings separated by **50 ms** (`readButton`) and detects a *released → pressed* transition (`checkButtonState`). `turnLedOnOrOff` updates `PB0` accordingly.

**Registers:** same as `ex02`: `DDRB`, `PORTB`, `DDRD`, `PORTD`, and `PIND`. Debouncing is implemented in blocking software, not through a hardware interrupt.

---

## 5. Module 01 — Timers and PWM

**Goal:** generate periodic signals using timer hardware instead of CPU-dependent busy loops, then control LED brightness through **PWM** (*Pulse Width Modulation*).

### `ex00` — Blink an LED Without a Hardware Timer

[View source](Mod_01/ex00/main.c). A `for (volatile uint32_t ...)` loop keeps the CPU busy before toggling `PB1` using `PORTB ^= ...`. The intended delay is roughly 0.5 s between transitions, corresponding to an approximately 1 Hz full on/off cycle.

**Registers:** only `DDRB` and `PORTB`. **Limitation:** the exact delay depends on generated machine instructions, optimization, and actual clock frequency. This is not a reliable timing source.

### `ex01` — Hardware Blinking with Timer1 in CTC Mode

[View source](Mod_01/ex01/main.c). The **16-bit Timer1** counts prescaled clock ticks. In **CTC** (*Clear Timer on Compare Match*) mode, the timer resets when its counter matches `OCR1A`. Its `OC1A` output pin (`PB1`) toggles automatically at every compare match.

```c
DDRB   |= (1 << PB1);
TCCR1B  = (1 << WGM12) | (1 << CS12); // CTC, prescaler 256
OCR1A   = 31249;
TCCR1A  = (1 << COM1A0);              // Toggle OC1A
```

At 16 MHz, with prescaler 256: `f_compare = 16,000,000 / (256 × (31249 + 1)) = 2 Hz`. The output toggles twice per second, meaning one complete on/off cycle lasts one second: **1 Hz blinking**.

**Registers:** `DDRB`; `TCCR1A` (`COM1A0`); `TCCR1B` (`WGM12`, `CS12`); `OCR1A` (compare threshold). No ISR is needed: the timer hardware changes the pin directly.

### `ex02` — Adjust Brightness with Fast PWM

[View source](Mod_01/ex02/main.c). Timer1 uses **Fast PWM mode 14**, `WGM13:0 = 1110`: `ICR1` defines TOP, `COM1A1` selects non-inverting `OC1A` output, and `OCR1A` sets the high-time portion of the cycle.

```c
TCCR1A = (1 << WGM11) | (1 << COM1A1);
TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12);
ICR1   = 62499;
OCR1A  = 6250;
```

PWM frequency: `16 MHz / (256 × (62499 + 1)) = 1 Hz`. Approximate duty cycle: `6250 / 62500 = 10%`. **Caution:** 1 Hz is much too slow for smooth perceived brightness; the LED may visibly flash instead of appearing steadily dimmed.

**Registers:** `TCCR1A`, `TCCR1B`, `ICR1`, `OCR1A`, `DDRB`.

### `ex03` — Adjust PWM with Two Buttons

[View source](Mod_01/ex03/main.c). This extends Timer1 Fast PWM with `ICR1 = 62499`. Two buttons connected to `PD2` and `PD4`, sampled with a 20 ms debounce delay, raise or lower the duty cycle in **10% steps**. Brightness starts at 10%, increases up to 100%, and decreases no lower than 10%.

The program computes the compare value using `62500 * newPourcent / 100` and writes it to `OCR1A` through an intermediate variable. **GPIO registers:** `DDRD`, `PORTD`, `PIND`, `DDRB`. **Timer1 registers:** `TCCR1A`, `TCCR1B`, `ICR1`, `OCR1A`. **Caution:** at 100%, `OCR1A = 62500` exceeds `TOP = 62499`; the PWM frequency is still 1 Hz.

---

## 6. Module 02 — UART Communication

**Goal:** exchange characters with a computer serial terminal through the ATmega328P's **USART0**, using **115200 baud, 8 data bits, no parity, and one stop bit (8N1)**.

### Common USART0 Configuration

```c
#define F_CPU 16000000UL
#define UART_BAUDRATE 115200
#define UART_BAUD_DIVIDER (F_CPU / (8 * UART_BAUDRATE) - 1)

UBRR0L = UART_BAUD_DIVIDER;               // Baud-rate divisor
UCSR0B |= (1 << TXEN0) | (1 << RXEN0);   // Enable TX and RX
UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, 8N1
UCSR0A |= (1 << U2X0);                  // Double-speed mode
```

With `U2X0 = 1`, the formula is `baud = F_CPU / (8 × (UBRR0 + 1))`. Here `UBRR0 = 16`, giving approximately **117,647 baud**, about **+2.1%** above the requested 115200. Keep this discrepancy in mind when diagnosing corrupted serial characters.

### `ex00` — Transmit a Character Periodically

[View source](Mod_02/ex00/main.c). The firmware sends the character `Z` once per second. `uart_tx` waits until `UDRE0` indicates that the transmit buffer is ready, then writes the byte to `UDR0`.

**Registers:** `UBRR0L` (baud divisor); `UCSR0A` (`U2X0`, `UDRE0`); `UCSR0B` (`TXEN0`); `UCSR0C` (`UCSZ01`, `UCSZ00`); `UDR0` (data). The example also uses masks for UART frame-format bits; parenthesized masks make these operations easier to read and less error-prone.

### `ex01` — Print “Hello World!” from a Timer1 Interrupt

[View source](Mod_02/ex01/main.c) · [Timer code](Mod_02/ex01/timer.c). Timer1 triggers `TIMER1_COMPA_vect`, whose interrupt handler sends `Hello World!\r\n` through UART. `sei()` enables global interrupts.

**Registers:** `OCR1A = 31249` (comparison value); `TCCR1B` (`WGM12`, `CS12`, `CS10`); `TIMSK1` (`OCIE1A`); plus the USART0 registers. **Caution:** a prescaler of `1024` with `OCR1A = 31249` produces approximately **two seconds** between interrupts, not one. Printing from an ISR is blocking and should be avoided in more demanding real-time applications.

### `ex02` — UART Echo

[View source](Mod_02/ex02/main.c). The firmware waits for `RXC0`, reads the received byte from `UDR0`, waits until `UDRE0` is set, and sends the same byte back. This tests both UART reception and transmission.

**Registers:** `UCSR0A` (`RXC0`, `UDRE0`, `U2X0`), `UCSR0B` (`RXEN0`, `TXEN0`), `UCSR0C`, `UBRR0L`, `UDR0`. Reception uses **polling** rather than interrupts.

### `ex03` — Interrupt-Driven UART Reception

[View source](Mod_02/ex03/main.c). `RXCIE0` enables the `USART_RX_vect` interrupt. Its handler reads the received byte from `UDR0`, changes the case of ASCII letters (`A` → `a`, `a` → `A`), and echoes the result. Other characters are forwarded unchanged.

**Registers:** the standard USART0 registers plus `UCSR0B.RXCIE0`. **Caution:** `uart_tx()` busy-waits inside the ISR; a transmission queue would be preferable in a larger application.

---

## 7. Module 03 — RGB LEDs and Serial Commands

**Goal:** mix colors using red (`PD5`), green (`PD6`), and blue (`PD3`) channels, then choose a color from a serial terminal.

### `ex00` — Three Primary Colors

[View source](Mod_03/ex00/main.c). `DDRD` configures `PD3`, `PD5`, and `PD6` as outputs. Every second, `PORTD` selects red, green, or blue. These assignments **overwrite all of `PORTD`**, potentially affecting other outputs and pull-up settings on the same port.

**Registers:** `DDRD`, `PORTD`.

### `ex01` — Seven Color Combinations

[View source](Mod_03/ex01/main.c). Adds red+green, green+blue, red+blue, and all three channels together. This is still simple **on/off** control, with no intermediate intensity values. **Registers:** `DDRD`, `PORTD`.

### `ex02` — Hardware-PWM RGB Gradient

[View source](Mod_03/ex02/main.c). Timer0 and Timer2 are configured for **8-bit Fast PWM**:

| Color | AVR pin | PWM output | Duty-cycle register |
|:--|:--|:--|:--|
| Red | `PD5` | `OC0B` | `OCR0B` |
| Green | `PD6` | `OC0A` | `OCR0A` |
| Blue | `PD3` | `OC2B` | `OCR2B` |

`TCCR0A` and `TCCR2A` select Fast PWM (`WGM00/01`, `WGM20/21`) and non-inverting outputs (`COM0A1`, `COM0B1`, `COM2B1`). `TCCR0B` uses a **64** prescaler; `TCCR2B` also uses **64** for Timer2 (`CS22`). At 16 MHz and 256 counts per period, PWM frequency is approximately **976.6 Hz**. `wheel(pos)` maps a position in the 0–254 range to RGB values to generate a gradual color cycle.

### `ex03` — Enter an RGB Color as Hexadecimal over UART

[View source](Mod_03/ex03/main.c) · [Parser](Mod_03/ex03/parsing.c) · [UART functions](Mod_03/ex03/uart.c). This exercise builds on PWM and reads the serial input one character at a time. It accepts **`#RRGGBB`**, for example `#FF0080`. `parse_input()` requires exactly seven characters and recognizes decimal digits and **uppercase** hexadecimal letters `A–F`. `get_rgb()` converts each pair to one byte (`0–255`), and `set_rgb()` writes the values to the OCR registers.

**Registers:** `DDRD`, `TCCR0A/B`, `TCCR2A/B`, `OCR0A`, `OCR0B`, `OCR2B`, and UART registers `UBRR0L`, `UCSR0A/B/C`, `UDR0`. The `str_input[8]` buffer supports seven meaningful characters plus terminator; extra input is ignored until the end of the line.

---

## 8. Module 04 — Interrupts

**Goal:** use **Interrupt Service Routines (ISRs)** to respond to hardware events instead of continuously polling inputs.

### `ex00` — External Interrupt INT0

[View source](Mod_04/ex00/main.c). A button on `PD2` is configured as a pull-up input, and `PB0` as an LED output. Initially, `EICRA.ISC01 = 1`, `ISC00 = 0` selects a **falling-edge** trigger on `INT0`. `EIMSK.INT0` enables the interrupt; `EIFR.INTF0` acknowledges a pending event when written with 1. The handler toggles the LED under a condition and changes `ISC00` to alter the selected edge.

**Registers:** `DDRB`, `PORTB`, `DDRD`, `PORTD`, `EICRA`, `EIMSK`, `EIFR`. **Implementation note:** the ISR begins with `EIMSK |= INT0`, enabling rather than disabling the interrupt source, waits 30 ms inside the handler, and changes edge-selection bits. This educational approach should be revisited for dependable debouncing.

### `ex01` — Periodically Adjust a PWM Duty Cycle

[View source](Mod_04/ex01/main.c). Timer0 operates in CTC mode (`TCCR0A.WGM01`) and triggers `TIMER0_COMPA_vect` via `TIMSK0.OCIE0A`. A counter moves up or down between 0 and 50. Its value is converted into `OCR1A` (`0–255`) to control 8-bit **Timer1 PWM** on `PB1`.

**Registers:** `TCCR0A`, `TCCR0B`, `OCR0A`, `TIMSK0` for the timing source; `TCCR1A`, `TCCR1B`, `OCR1A` for PWM; `DDRB` for the output. Timer0 uses prescaler 1024 (`CS02|CS00`) and `OCR0A = 155`, giving approximately 10 ms between interrupts at 16 MHz.

### `ex02` — Four-Bit Counter with Two Buttons

[View source](Mod_04/ex02/main.c). Two buttons (`PD2`, `PD4`) increment or decrement a wraparound counter from **0 to 15**. Four LEDs (`PB0`, `PB1`, `PB2`, `PB4`) show its binary value. `PD2` uses `INT0`; `PD4` uses **pin-change interrupt** `PCINT20` (PCINT2 group). When either event occurs, `start_timer()` starts Timer1; `TIMER1_COMPA_vect` reads and stores button states, updates the counter, and stops the timer once both buttons are released.

**Registers:** `DDRB`, `PORTB`, `DDRD`, `PORTD`, `PIND`; `EICRA`, `EIMSK`, `EIFR` for `INT0`; `PCICR.PCIE2`, `PCMSK2.PCINT20`, `PCIFR.PCIF2` for pin-change detection; and `TCCR1B`, `TCNT1`, `OCR1A`, `TIMSK1` for Timer1. CTC mode with a 1024 prescaler and compare value `312` gives about 20 ms between comparisons.

---

## 9. Module 05 — Analog-to-Digital Conversion

**Goal:** use the 10-bit ADC to digitize voltages from a potentiometer, light sensor, and temperature input, then report the values through UART or display them on LEDs.

### How the ADC Works

`ADMUX` chooses the voltage reference (`REFS1:0`), channel (`MUX3:0`), and result alignment (`ADLAR`). `ADCSRA` enables the ADC (`ADEN`), begins a conversion (`ADSC`), exposes the completion flag (`ADIF`), and sets its clock prescaler (`ADPS2:0`). Here `ADPS2:0 = 111` means prescaler **128**, so `f_ADC = 16 MHz / 128 = 125 kHz`.

A 10-bit result lies between `0` and `1023`. With `ADLAR = 1`, reading `ADCH` alone produces an **8-bit** value from `0` to `255`. With `ADLAR = 0`, read **`ADCL` before `ADCH`** to retrieve the full 10-bit result. With AVcc as reference (`REFS0 = 1`, `REFS1 = 0`), the result represents input voltage relative to the analog supply.

### `ex00` — Read ADC0 as an 8-Bit Value

[View source](Mod_05/ex00/main.c). Select **ADC0**, start each conversion using `ADSC`, wait for completion, and read `ADCH`. A helper converts the byte into two hexadecimal characters, which are sent over UART about every 20 ms.

**Registers:** `ADMUX` (`REFS0`, `ADLAR`, `MUX0–3`); `ADCSRA` (`ADEN`, `ADPS0–2`, `ADSC`); `ADCH`; and the UART registers.

### `ex01` — Read Three ADC Channels as 8-Bit Values

[View source](Mod_05/ex01/main.c). Sequentially sample **ADC0 (potentiometer)**, **ADC1 (light)**, and **ADC2 (temperature)** as intended by the circuit. `adc_return_value(channel)` clears the MUX bits, selects the channel, starts conversion, and returns `ADCH`. All three bytes are printed in hexadecimal.

**Registers:** `ADMUX`, `ADCSRA`, `ADCH`, and USART0 registers. **Caution:** after changing channels, a settling or dummy conversion may be needed, depending on sensor impedance and acquisition time.

### `ex02` — Read Three Channels at Full 10-Bit Resolution

[View source](Mod_05/ex02/main.c). This expands `ex01` by selecting `ADLAR = 0`, reading `ADCL` and then `ADCH`, and combining the bytes as `value = (high << 8) | low`. Each reading is printed in decimal using four digits (`0000`–`1023`).

**Registers:** `ADMUX` (`ADLAR=0`, `MUX0–3`, `REFS0`); `ADCSRA` (`ADEN`, `ADPS0–2`, `ADSC`, `ADIF`); `ADCL`, `ADCH`; and UART. The code checks `ADIF` after `ADSC` completes. Remember that `ADIF` is cleared by writing **1**, not 0; its handling could be simplified.

### `ex03` — ATmega328P Internal Temperature Sensor

[View source](Mod_05/ex03/main.c). Enable the **internal 1.1 V reference** (`REFS1|REFS0`), select the internal temperature channel using `MUX3`, read a 10-bit ADC result, and use the exercise's conversion formula:

```text
T (°C) = ADC × (100 / 128) − 250
```

The computed temperature is transmitted over UART. **Important:** this equation is an approximation used by the exercise. The internal AVR temperature sensor requires **calibration** for accurate absolute readings and time for the reference voltage to stabilize. It measures **chip temperature**, not ambient air temperature.

**Registers:** `ADMUX`, `ADCSRA`, `ADCL`, `ADCH`, and USART0.

### `ex04` — Potentiometer-Driven LED Gauge and RGB Color

[View source](Mod_05/ex04/main.c) · [LED utilities](Mod_05/ex04/led.c). The potentiometer on ADC0 produces an 8-bit value. `led_jauge()` activates four LEDs (`PB0`, `PB1`, `PB2`, `PB4`) as thresholds `64`, `128`, `192`, and `255` are reached. `wheel(value)` simultaneously computes an RGB color using hardware PWM.

**Registers:** `ADMUX`, `ADCSRA`, `ADCH`; `DDRB`, `PORTB`, `DDRD`; `TCCR0A/B`, `TCCR2A/B`, `OCR0A`, `OCR0B`, `OCR2B`. The first three gauge stages cover quarter-scale intervals; the fourth LED turns on only at maximum value 255.

---

## 10. Module 06 — I²C Bus and AHT20 Sensor

**Goal:** communicate with an **AHT20** digital temperature/humidity sensor over I²C, called **TWI** (*Two-Wire Interface*) in AVR documentation.

### Understanding the Bus

On the ATmega328P, I²C uses **SDA (`PC4`)** for data and **SCL (`PC5`)** for the clock. These lines require pull-up resistors appropriate to the supply voltage and circuit. The AHT20 uses the **7-bit address `0x38`**. The transmitted address byte is `(0x38 << 1) | R/W`, where the final bit selects write (`0`) or read (`1`).

`TWBR`, together with `TWSR.TWPS1:0`, determines the bit rate. `TWCR` requests START/STOP conditions, enables TWI, controls ACK/NACK, and reports progress with `TWINT`. `TWDR` holds transmitted and received bytes. Status codes come from the status field of `TWSR`, masked by the `TW_STATUS` macro from `<util/twi.h>`.

With `TWBR = 72` and prescaler 1: `SCL ≈ 16 MHz / (16 + 2 × 72) = 100 kHz`.

### `ex00` — Detect the AHT20 Sensor

[View source](Mod_06/ex00/main.c). Initialize TWI, issue START, transmit `0x38 << 1` in write mode, and compare the returned status against `TW_MT_SLA_ACK`. Report over UART whether the device acknowledged its address, send STOP, and wait roughly two seconds before retrying.

**Registers:** `TWSR` (`TWPS0/1`), `TWBR`, `TWCR` (`TWINT`, `TWEN`, `TWSTA`, `TWSTO`), `TWDR`, `TCNT1`, `TCCR1B` for waiting, and USART0. **Caution:** `timer_2s()` starts Timer1 in CTC mode without setting its TOP register `OCR1A` in this exercise. Actual timing may not match the intended delay; verify on the board.

### `ex01` — Read Seven Raw Measurement Bytes

[View source](Mod_06/ex01/main.c) · [Utilities](Mod_06/ex01/utils.c). After checking sensor status, the program starts a measurement with **`0xAC 0x33 0x00`**, waits 80 ms, reads **seven bytes**, and prints their hexadecimal values. During reception, the master sends ACK for the first six bytes (`TWEA=1`) and NACK for the final byte (`TWEA=0`) before STOP. If the calibration status is incomplete, this implementation issues **`0xBE 0x08 0x00`** as an initialization sequence.

**Registers:** `TWSR`, `TWBR`, `TWCR`, `TWDR`, and USART0 registers. `TW_START`, `TW_MT_SLA_ACK`, `TW_MR_SLA_ACK`, `TW_MT_DATA_ACK`, `TW_MR_DATA_ACK`, and `TW_MR_DATA_NACK` are protocol status constants, **not hardware registers**.

### `ex02` — Calculate Temperature and Humidity

[View source](Mod_06/ex02/main.c) · [Utilities](Mod_06/ex02/utils.c). Reuse the I²C communication routines, convert raw bytes into physical measurements, and print a moving average covering up to **three samples**.

The sensor's measurement frame packs 20 bits for each value:

```text
raw_humidity    = (byte1 << 12) | (byte2 << 4) | (byte3 >> 4)
raw_temperature = ((byte3 & 0x0F) << 16) | (byte4 << 8) | byte5

Humidity (%)    = raw_humidity × 100 / 2^20
Temperature (°C) = raw_temperature × 200 / 2^20 − 50
```

Helper routines format the values before UART transmission. **Registers:** TWI (`TWBR`, `TWSR`, `TWCR`, `TWDR`) and USART0. The seventh byte contains the CRC, but the current implementation **does not validate it**. A robust driver should also inspect the sensor's *busy* flag rather than relying entirely on a fixed delay.

---

## 11. Module 07 — EEPROM Memory

**Goal:** understand the microcontroller's internal EEPROM, which retains data when power is removed. The ATmega328P has **1024 bytes** of EEPROM, addressed from `0x000` to `0x3FF`.

### `ex00` — Work-in-Progress EEPROM Hex Dump

[View source](Mod_07/ex00/main.c). Comments describe a utility intended to read EEPROM as **16-byte rows**, showing addresses, hexadecimal values, and printable ASCII characters. The current code defines an empty `init_eeprom()` function and attempts to read data with `eeprom_read_byte()`.

**Actual status: unfinished.** The variable `ligne` is used without being declared; the argument passed to `eeprom_read_byte` is not converted to a valid EEPROM address pointer; the outer loop advances by one byte rather than one 16-byte row; the final iterations would read beyond the `0–1023` address range; and no UART hex-dump output has been implemented yet. This exercise should not be considered operational.

### EEPROM Hardware Registers

| Register | Purpose |
|:--|:--|
| `EEARL` | Low eight bits of EEPROM address |
| `EEARH` | High address bits (only two extra bits are needed on ATmega328P) |
| `EEAR` | 16-bit address view combining `EEARH:EEARL` |
| `EEDR` | Byte to program, or byte returned by a read |
| `EECR` | EEPROM operation control and status |

Within `EECR`, `EERE` initiates a read; `EEPE` starts and indicates a write in progress; `EEMPE` briefly enables the protected programming sequence; `EEPM1:0` selects erase/write modes; and `EERIE` enables the EEPROM-ready interrupt. A low-level read sets `EEAR`, waits for any active write to finish (`EEPE=0`), triggers `EERE`, and reads `EEDR`. Writing requires the protected `EEMPE` → `EEPE` sequence within the timing limits specified by the manufacturer.

`<avr/eeprom.h>` provides higher-level routines such as `eeprom_read_byte()` and `eeprom_update_byte()`, avoiding much of the direct hardware sequencing. **In the provided source, `EEAR`, `EEDR`, and `EECR` are chiefly discussed in comments; this exercise does not implement direct access to those registers.**

---

## 12. Register Reference

The table below summarizes relevant registers appearing **in the source code or its explanatory comments**, together with the bits used by the exercises. Descriptions apply to the ATmega328P.

| Peripheral | Register | Role and relevant bits |
|:--|:--|:--|
| GPIO | `DDRB` | Direction of PB0, PB1, PB2, PB4; `1` = output |
| GPIO | `PORTB` | LED output levels; used with `|=`, `&= ~`, `^=` |
| GPIO | `DDRD` | Direction of button and RGB pins on PORTD |
| GPIO | `PORTD` | RGB outputs or pull-ups for buttons on PD2/PD4 |
| GPIO | `PIND` | Digital input levels on PD2/PD4 |
| Timer0 | `TCCR0A` | `WGM00`, `WGM01` for Fast PWM/CTC; `COM0A1`, `COM0B1` for PWM outputs |
| Timer0 | `TCCR0B` | `CS00`, `CS01`, `CS02`: Timer0 prescaler |
| Timer0 | `OCR0A` | Channel A compare; TOP in CTC or OC0A PWM duty cycle in Fast PWM |
| Timer0 | `OCR0B` | OC0B PWM duty cycle |
| Timer0 | `TIMSK0` | `OCIE0A`: enable compare-match A interrupt |
| Timer1 | `TCCR1A` | `COM1A0` toggles OC1A; `COM1A1` non-inverting PWM; `WGM10/11` waveform modes |
| Timer1 | `TCCR1B` | `WGM12/13` waveform mode; `CS10/11/12` prescaler |
| Timer1 | `TCNT1` | Current 16-bit timer count |
| Timer1 | `OCR1A` | Channel A compare: CTC threshold, PWM duty cycle, or interrupt trigger |
| Timer1 | `ICR1` | TOP in Fast PWM mode 14 |
| Timer1 | `TIMSK1` | `OCIE1A`: interrupt when `TCNT1 == OCR1A` |
| Timer2 | `TCCR2A` | `WGM20`, `WGM21`: Fast PWM; `COM2B1`: non-inverting OC2B |
| Timer2 | `TCCR2B` | `CS22`: prescaler 64 in synchronous Timer2 mode |
| Timer2 | `OCR2B` | Blue PWM duty cycle on OC2B |
| UART | `UBRR0L`, `UBRR0H` | Low/high baud-rate divisor bytes; code mainly writes `UBRR0L` |
| UART | `UCSR0A` | `U2X0` double speed; `UDRE0` TX buffer ready; `RXC0` received byte ready |
| UART | `UCSR0B` | `TXEN0`, `RXEN0` enable TX/RX; `RXCIE0` enables receive ISR |
| UART | `UCSR0C` | `UCSZ01`, `UCSZ00` for 8 bits; `UMSEL0x`, `UPM0x`, `USBS0` for mode/parity/stop |
| UART | `UDR0` | Write to transmit a byte; read to retrieve received data |
| Interrupts | `EICRA` | `ISC01:ISC00` select INT0 triggering condition |
| Interrupts | `EIMSK` | `INT0` external interrupt enable bit |
| Interrupts | `EIFR` | `INTF0` pending-event flag, cleared by writing 1 |
| Interrupts | `PCICR` | `PCIE2` enables pin-change interrupts for PORTD group |
| Interrupts | `PCMSK2` | `PCINT20` enables changes on PD4 |
| Interrupts | `PCIFR` | `PCIF2` pin-change group 2 flag, cleared by writing 1 |
| ADC | `ADMUX` | `REFS1:0` reference, `ADLAR` alignment, `MUX3:0` channel |
| ADC | `ADCSRA` | `ADEN` enable; `ADSC` start; `ADIF` complete; `ADPS2:0` prescaler |
| ADC | `ADCL` | Low ADC result byte; read before `ADCH` |
| ADC | `ADCH` | High ADC result byte; yields 8-bit value if `ADLAR=1` |
| I²C/TWI | `TWBR` | Bus bit rate, together with TWSR prescaler |
| I²C/TWI | `TWSR` | `TWPS1:0` prescaler; status field accessed through `TW_STATUS` |
| I²C/TWI | `TWCR` | `TWEN` enable, `TWINT` operation completion/restart, `TWSTA` START, `TWSTO` STOP, `TWEA` ACK |
| I²C/TWI | `TWDR` | Data or address byte on the bus |
| EEPROM | `EEARH`, `EEARL`, `EEAR` | EEPROM address (described in comments) |
| EEPROM | `EEDR` | EEPROM data (described in comments) |
| EEPROM | `EECR` | `EERE`, `EEPE`, `EEMPE`, `EEPM1:0`, `EERIE` (described in comments) |

### Registers vs. Bits vs. Interrupt Vectors

A **register** is a hardware storage location such as `UCSR0A`. A **bit name** such as `UDRE0` identifies one field in a register. An **interrupt vector** such as `USART_RX_vect`, `TIMER1_COMPA_vect`, `TIMER0_COMPA_vect`, `INT0_vect`, or `PCINT2_vect` identifies the handler executed for an event. `sei()` enables global interrupts by setting the CPU status register's I flag (`SREG`). Finally, `TW_STATUS`, `TW_START`, and `TW_MR_DATA_ACK` are protocol macros/constants defined in `<util/twi.h>`, **not separate registers**.

### Useful Formulas

```text
CTC timer:
  f_compare = F_CPU / (prescaler × (1 + OCRnA))
  If the output toggles on every compare match: f_signal = f_compare / 2

Fast PWM with TOP = ICR1:
  f_PWM = F_CPU / (prescaler × (1 + ICR1))
  duty cycle ≈ OCR1A / (ICR1 + 1)

Asynchronous USART, double-speed mode:
  actual_baud = F_CPU / (8 × (UBRR0 + 1))

I²C/TWI:
  f_SCL = F_CPU / (16 + 2 × TWBR × TWI_prescaler)

ADC conversion clock:
  f_ADC = F_CPU / ADC_prescaler
```

---

## 13. Known Limitations and Improvements

This repository is a **learning project**, not production-validated hardware firmware. Reviewing the source reveals several areas to improve:

1. **Clock and timing accuracy:** double-check prescaler and TOP calculations. `Mod_02/ex01` triggers about every two seconds, while `Mod_01/ex02` uses a 1 Hz PWM signal.
2. **Short interrupt handlers:** move blocking delays and serial transmissions out of ISRs where possible, using `volatile` flags or ring buffers instead.
3. **Safe port updates:** avoid overwriting a whole register (`PORTD = ...`) if other functions share the same port.
4. **ADC reliability:** allow channel/reference settling time, calibrate temperature readings, and document electrical assumptions.
5. **I²C robustness:** add timeouts, error/status handling, AHT20 *busy* checks, and CRC validation.
6. **EEPROM:** finish the hex-dump exercise, correct addresses, keep every read within `0x000–0x3FF`, and implement serial output.
7. **Portability:** centralize hardware constants and make serial port, programmer, and clock settings configurable without editing every Makefile.

These notes describe the supplied project sources and **do not** represent results from testing on a physical board.

## 14. References

- [Microchip — ATmega328P product documentation](https://www.microchip.com/en-us/product/ATmega328P): authoritative descriptions of registers and operating modes.
- [AVR-LibC documentation](https://www.nongnu.org/avr-libc/user-manual/): `<avr/io.h>`, `<avr/interrupt.h>`, `<util/delay.h>`, `<util/twi.h>`, and `<avr/eeprom.h>`.
- [AVRDUDE documentation](https://avrdudes.github.io/avrdude/): flashing and reading AVR microcontrollers.

---

<div align="center">

**Embedded Systems Piscine — understanding hardware, one register at a time.**

</div>
