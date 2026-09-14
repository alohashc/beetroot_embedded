# Lesson 3.1 ESP-IDF Project

This project demonstrates a simple ESP-IDF application that reads an analog input, smooths it with a simple moving-average filter, and uses hysteresis to control an LED.

## Overview

https://github.com/user-attachments/assets/f6665a4c-c17a-4363-9b1e-2318de3813e4

The firmware does the following:

- Initializes ADC1 on the ESP32
- Configures an ADC channel with calibration
- Reads the raw ADC value and converts it to millivolts
- Applies a 5-point moving average (SMA)
- Turns an LED on when the average voltage drops below the lower threshold
- Turns the LED off when the average voltage rises above the upper threshold

This creates a simple threshold detector with hysteresis so the LED state does not flicker around the threshold.

## Build

From this folder:

```bash
pio run
```

Or explicitly:

```bash
pio run -e esp32-s3-devkitc-1
```

## Upload

```bash
pio run -t upload
```

Or:

```bash
pio run -e esp32-s3-devkitc-1 -t upload
```

## Serial monitor

```bash
pio device monitor
```

Or via PlatformIO target:

```bash
pio run -t monitor
```

## Expected behavior

When the analog input voltage is low enough, the LED turns on. As the voltage rises above the upper threshold, the LED turns off. The moving-average filter reduces noise and short spikes.

[Back to main project README](../README.md)
