# Lesson 4.3: STM32 RTC and OLED Display

## Project overview

This project targets an STM32F411CEU6 Black Pill board and uses STM32Cube HAL through PlatformIO's `stm32cube` framework. It reads the current time and date from a DS1307 real-time clock and displays them on a 128×64 SSD1306 OLED. The display is refreshed once per second. RTC read failures are reported over USB CDC.

## Hardware and connections

The DS1307 and SSD1306 share I2C1:

| STM32F411 pin | I2C signal |
| --- | --- |
| PB6 | SCL |
| PB7 | SDA |

The devices use the 7-bit I2C addresses `0x68` (DS1307) and `0x3C` (SSD1306); the firmware passes the HAL's left-shifted address form to the I2C APIs. Confirm the supply-voltage and pull-up requirements of your RTC and display modules before connecting them.

## Behavior

- Initializes the SSD1306 display and reads the DS1307 time once per second.
- Displays the time in `HH:MM:SS` format and the weekday/date below it.
- Prints `DS1307 read error!` to the USB CDC serial interface when a read fails.
- Provides `DS1307_SetTime()` in the application library for writing a time, although the main application currently only reads the RTC.
- Reads and writes time in 24-hour format for years 2000–2099.

## Project structure

- `Core/Src/app/main_cpp.cpp` — application loop and OLED rendering
- `Core/Src/app/ds1307.cpp` and `Core/Inc/app/ds1307.h` — DS1307 time read/write functions
- `lib/ssd1306/` — SSD1306 display driver and configuration
- `USB_DEVICE/` — USB CDC device support
- `platformio.ini` — PlatformIO build and ST-Link upload configuration

## Build and upload

From this project directory, build the default `blackpill_f411ce` environment:

```sh
pio run
```

Upload to the board using the configured ST-Link protocol:

```sh
pio run -t upload
```

[Back to the main project README](../README.md)
