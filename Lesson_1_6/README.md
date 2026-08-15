# Lesson 1.6 — ADC Voltage Monitor with Hysteresis LED Control

## Overview
This project demonstrates how to read an analog input on an ESP32, convert the raw ADC value into a voltage estimate, compare it with the hardware-calibrated millivolt reading, and use a hysteresis threshold to control an output LED.

The firmware is implemented in [src/main.cpp](src/main.cpp). It reads ADC input on GPIO 4, converts the signal, logs the readings over Serial, and toggles an LED on GPIO 15 depending on whether the measured voltage is below or above set thresholds.

## What the firmware does

It performs the following steps in each loop iteration:
1. Reads the raw ADC value with `analogRead(ADC_IN)`.
2. Reads the calibrated ADC value in millivolts with `analogReadMilliVolts(ADC_IN)`.
3. Converts the raw value into a voltage estimate using:
   - `raw / 4095 * 3100 mV`
4. Compares the raw-derived value and calibrated value by calculating the percentage delta.
5. Prints the measurement details to the serial monitor.
6. Uses hysteresis to keep the LED state stable around a threshold:
   - below `1800 mV - 100 mV = 1700 mV` -> LED turns ON
   - above `1800 mV + 100 mV = 1900 mV` -> LED turns OFF

This prevents rapid flickering when the signal hovers around the decision point.

## Key concepts
- ADC conversion on ESP32
- Voltage estimation from raw ADC counts
- Calibrated millivolt readings using `analogReadMilliVolts()`
- Hysteresis-based threshold logic
- Serial debugging output for analog measurements

## Build and run
This project is set up for PlatformIO.

1. Open the folder in VS Code.
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

## Video

https://github.com/user-attachments/assets/8c46f8af-86ef-4049-912e-86f080ecc725



## Notes
The sketch is useful as a practical example of measuring analog voltage and using threshold logic to create a stable digital control response. It is a good stepping stone for sensor reading, voltage monitoring, or simple comparator-style behavior on an ESP32.

[Back to main project README](../README.md)
