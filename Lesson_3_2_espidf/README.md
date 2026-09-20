# Lesson 3.2: ESP32-S3 ADC Voltage Measurement

This PlatformIO project demonstrates ADC measurements on an ESP32-S3 using the
ESP-IDF framework. It reads the ADC value in two ways: as a raw 12-bit reading
and as a calibrated voltage in millivolts.

## Hardware

- Board: ESP32-S3 DevKitC-1 (YD-ESP-S3 compatible)
- ADC unit: ADC1
- ADC channel: channel 3
- Default input pin: GPIO4
- ADC attenuation: 12 dB
- Serial baud rate: 115200

Connect the analog signal to GPIO4 and share a ground with the board. Keep the
input voltage within the ESP32-S3 ADC input range.

## What the example does

The application:

1. Initializes the ESP-IDF oneshot ADC driver.
2. Configures ADC1 channel 3 with 12 dB attenuation and the default bit width.
3. Reads the raw ADC value.
4. Reads the calibrated voltage in millivolts using the ADC calibration helper.
5. Logs the manual voltage estimate, calibrated voltage, and percentage error
	every 250 ms.

The ADC helper is implemented in `lib/adc/` and is linked through the project's
ESP-IDF CMake configuration.

## Build

Run these commands from this project directory:

```bash
pio run
```

## Upload and monitor

Upload the firmware:

```bash
pio run -t upload
```

Start the serial monitor:

```bash
pio device monitor
```

The monitor can also be started through the PlatformIO target:

```bash
pio run -t monitor
```

Expected log output has this form:

```text
RAW: 1234 | U_manual: 1325.4 mV | U_cali: 1188 mV | Error: 11.57%
```

[Back to main project README](../README.md)
