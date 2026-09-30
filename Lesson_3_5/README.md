# Lesson 3.5 — Servo Control from ADC Input

## Project Overview

This project demonstrates a simple ESP32-based closed-loop servo controller. A potentiometer connected to ADC input is read on GPIO4, and the measured value is converted into a servo angle from 0 to 180 degrees. The servo signal is generated on GPIO15 using a PWM signal configured for a standard hobby servo timing pattern.

The program continuously samples the ADC, maps the raw value to an angle, and updates the servo position every 100 ms.

## Hardware

- MCU: ESP32
- Servo signal pin: GPIO15
- ADC input channel: ADC1_CH3 (GPIO4)
- Potentiometer: connected to ADC input for manual angle control
- Servo: standard 180° hobby servo compatible with PWM control

## Software Structure

The project is organized into a few core components:

- `src/main.cpp` — main application loop and ADC-to-servo control logic
- `lib/adc/` — ADC initialization and one-shot read helpers
- `lib/pwm/` — PWM generation for the servo signal
- `lib/servo/` — servo angle conversion and API wrapper

## How It Works

1. The ADC is initialized in one-shot mode with 12-bit resolution.
2. The ADC reads the potentiometer voltage on GPIO4.
3. The raw ADC value is scaled to a target angle range:
   - `0..4095` ADC range
   - mapped to `0..180` degrees
4. The servo is updated using the configured PWM output.
5. The current measured ADC value and resulting angle are printed to the serial console.

## Key Configuration

The project uses the following settings in `platformio.ini`:

- Framework: ESP-IDF
- Board: `esp32-s3-devkitc-1`
- Serial monitor speed: `115200`

## Build and Run

From this project folder, run:

```sh
pio run
```

To flash and monitor the board:

```sh
pio run --target upload --target monitor
```

## Expected Behavior

- Turning the potentiometer adjusts the servo angle.
- The serial output shows the raw ADC value and the resulting degree value.
- The servo follows the potentiometer smoothly within the 0° to 180° range.

## Notes

This project is a good starting point for ESP32 servo steering, actuator control, and sensor-driven motion tasks.
