# Lesson 4.1: UART Communication

This project demonstrates a simple UART-based command exchange between an ESP32-S3 and an STM32F411 Black Pill board.

The ESP32 sends a command when its button is pressed, and the STM32 toggles its onboard LED in response. The STM32 also monitors its own button and sends a command back to the ESP32, creating a two-way communication loop over UART.

## What this project does

- ESP32 button press sends the string `Change LED State` over UART
- STM32 receives the command and toggles the onboard LED connected to `PC13`
- STM32 button press sends the same command back to the ESP32
- Both sides communicate at `115200` baud, using a simple newline-terminated ASCII protocol

## Hardware connection

This project uses a direct UART connection between the boards:

- ESP32 TX (`GPIO17`) -> STM32 RX (`USART1_RX`)
- ESP32 RX (`GPIO18`) -> STM32 TX (`USART1_TX`)
- GND shared between both boards

## Project structure

- `src/esp32/main.c` — ESP32 application logic, UART setup, button handling, and LED control
- `src/stm32/main.c` — STM32 startup and peripheral initialization
- `src/stm32/app/app_main.cpp` — STM32 UART and button state machine
- `include/` — project headers for the STM32 build
- `lib/` — USB device library used by the STM32 firmware

## Build and run

From this project directory:

```bash
pio run -e esp32
pio run -e stm32
```

Upload the firmware for the selected target:

```bash
pio run -e esp32 -t upload
pio run -e stm32 -t upload
```

Monitor serial output:

```bash
pio device monitor
```

## Notes

- The default PlatformIO environment is `esp32`.
- The STM32 project uses the `blackpill_f411ce` board definition and initializes USB CDC for debug/serial output.
- This is a small embedded communication example intended to show UART message exchange between two microcontrollers.

[Back to main project README](../README.md)
