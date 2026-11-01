# Lesson 4.7: STM32 Debug and Button-Controlled LED Blink

## Project overview

https://github.com/user-attachments/assets/782e5627-d85d-4d84-a625-401a47feeb4e

This project targets an STM32F411CEU6 Black Pill board and uses the STM32Cube framework through PlatformIO. It demonstrates the basic debugging workflow for a bare-metal firmware project by flashing an LED and reacting to a hardware button with software debouncing.

The firmware configures PA5 as an output LED pin and PA0 as an input with an internal pull-up. Each time the button is pressed and stabilized, the LED blink interval changes in a cycle: 200 ms, 500 ms, then 1000 ms, and back to 200 ms.

This is a good starting point for learning:

- GPIO configuration for output and input pins
- internal pull-up resistors and button input reading
- software debounce logic using a timer tick
- state changes and timing in `while(1)` control loops
- debugging a simple embedded application with ST-LINK

## Hardware and connections

| STM32F411 pin | Signal | Device |
| --- | --- | --- |
| PA5 | GPIO output | On-board LED (or external LED with resistor) |
| PA0 | GPIO input | Push button with pull-up enabled |

The project uses an on-board LED connected to PA5 on many Black Pill boards. The button can be a simple tactile switch tied to PA0 and GND, with the internal pull-up enabled in firmware.

## Behavior

- Initializes GPIOA and configures PA5 as a push-pull output pin.
- Sets PA0 as an input with an internal pull-up resistor.
- Tracks the last raw button state and a debounced stable state using `uwTick`.
- When the button is pressed and stable, cycles the LED blink interval between 200 ms, 500 ms, and 1000 ms.
- Keeps the LED toggling continuously in the main loop while the button state is sampled.

## Project structure

- `Core/Src/app/app_main.cpp` — LED and button logic, debounce handling, and blink interval changes
- `Core/Inc/app/app_main.h` — application entry declaration
- `Core/Src/main.c` — generated main entry and HAL initialization
- `platformio.ini` — PlatformIO build configuration for the Black Pill STM32F411CEU6
- `STM32F411CEUX_FLASH.ld` and `STM32F411CEUX_RAM.ld` — linker scripts

## Build and upload

From this project directory, build the firmware:

```sh
pio run
```

Upload to the board:

```sh
pio run -t upload
```

To debug with ST-LINK, you can use VS Code + PlatformIO or open a debug session configured for the `blackpill_f411ce` environment.

[Back to the main project README](../README.md)
