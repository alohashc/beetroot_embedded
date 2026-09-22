# Lesson 3.3: ESP32-S3 ADC-Controlled PWM

This PlatformIO project demonstrates reading a potentiometer with the ESP-IDF
oneshot ADC driver and using the resulting level to control an LED and a motor
through PWM.

https://github.com/user-attachments/assets/a636846b-7d99-4ece-8d43-0240f27dcb7d


## Hardware

- Board: ESP32-S3 DevKitC-1
- Analog input: ADC1 channel 3 on GPIO4
- LED output: GPIO18
- Motor PWM output: GPIO15
- ADC attenuation: 12 dB
- PWM resolution: 8-bit
- Serial baud rate: 115200

Connect the potentiometer wiper to GPIO4 and its other terminals to 3.3 V and
GND. Share a common ground between all devices. Connect the LED through a
current-limiting resistor. Drive the motor through an appropriate transistor,
 or motor driver with a separate motor power supply;

## What the example does

The application:

1. Initializes ADC1 channel 3 in oneshot mode.
2. Reads the raw 12-bit ADC value every 200 ms.
3. Converts the reading to a 0-100% level.
4. Applies that level to the LED at 1 kHz and the motor at 20 kHz.

The ADC helper is implemented in `lib/adc/`, and the LEDC PWM helper is
implemented in `lib/pwm/`.

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

This example does not print periodic application values by default. The LED
brightness and motor speed should follow the potentiometer position.

[Back to main project README](../README.md)
