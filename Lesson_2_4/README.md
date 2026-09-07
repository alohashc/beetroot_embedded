# Lesson 2.4 - ESP32 Button Handling Experiments

This project demonstrates several ways to handle a button input on an ESP32 using PlatformIO. The examples compare a polling-based state machine with interrupt-driven techniques and show how mechanical bounce can affect count accuracy.

## Overview

The board reads a button on GPIO 15. The firmware is structured to compile one implementation at a time using compiler flags defined in `platformio.ini`.

The project includes three approaches:

- FSM polling with software debouncing
- simple interrupt counting
- interrupt counting with debounce filtering

## Hardware

- Board: ESP32-S3 DevKitC-1
- Button module: 3-pin module with built-in pull-up resistor (VCC, OUT, GND)
- Button input: GPIO 15 connected to the module output (`OUT`)
- Power: connect `VCC` to 3.3V and `GND` to GND
- Logic: active LOW (`digitalRead() == LOW` when pressed)
- Serial monitor: 115200 baud

## Build environments

Current configuration:

- `default_envs = isr_simple`

Available environments:

- `fsm` - finite state machine implementation
- `isr_simple` - interrupt-based version without debounce
- `isr_debounce` - interrupt-based version with debounce logic

## Build commands

Build the default environment:

```bash
pio run
```

Build the FSM version:

```bash
pio run -e fsm
```

Build the simple interrupt version:

```bash
pio run -e isr_simple
```

Build the debounced interrupt version:

```bash
pio run -e isr_debounce
```

Upload and monitor a specific environment:

```bash
pio run -e isr_debounce --target upload --target monitor
```

## Behavior by implementation

### 1. FSM version

This implementation uses a periodic polling loop with a small delay between samples. It tracks the button state through:

- IDLE
- debounce press
- pressed
- debounce release

A press is accepted only after the signal remains stable for a minimum time. This provides robust handling of contact bounce in software.

### 2. Simple ISR version

The interrupt routine increments a counter whenever the falling edge occurs. The main loop prints the count after each event flag is set.

This version is simple and fast, but it does not reject bounce-induced extra interrupts.

### 3. Debounced ISR version

The interrupt still increments a raw counter, but a time check in `loop()` filters out events that are too close together. This keeps the displayed count closer to the real number of user presses while still reacting quickly to edge-triggered input.

## Learning goals

This project is intended to explore:

- polling versus interrupt-driven input handling
- debounce strategies in firmware
- edge-triggered interrupt behavior on embedded systems
- practical differences between software and hardware filtering

## Notes

The code is intentionally simple and educational. It is best used as a reference for comparing basic input-handling patterns before moving to more advanced embedded designs.
