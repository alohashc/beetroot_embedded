# Lesson 1.4 — Debounced LED Control on ESP32

## Overview
This project demonstrates a simple embedded control system for an ESP32-based board. It uses two buttons and two LEDs to switch between different blinking patterns without blocking the program with `delay()`.

## What the firmware does
The sketch in [src/main.cpp](src/main.cpp) configures:
- a red LED on GPIO 18
- a blue LED on GPIO 17
- a user button on GPIO 15
- the BOOT button on GPIO 0

It supports three LED modes:
- `FAST` — alternating red/blue blinking at a short interval
- `SLOW` — alternating red/blue blinking at a longer interval
- `SYNC` — both LEDs blinking together

## Key features
- Button debouncing to avoid false triggers from contact noise
- Non-blocking timing using `millis()` for LED blinking
- Serial output for button presses and current mode changes
- Simple state machine for mode selection

## Build and run
This project is set up for PlatformIO.

1. Open the project folder in VS Code.
2. Build the firmware:
   ```bash
   pio run
   ```
3. Upload it to the board:
   ```bash
   pio run --target upload
   ```
4. Open the serial monitor:
   ```bash
   pio run --target monitor
   ```

## Notes
The project is a good example of basic input handling, debounce logic, and state-based LED control on an ESP32 device.
