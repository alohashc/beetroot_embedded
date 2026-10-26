# Lesson 4.4: STM32 RTC and Environmental Sensor Display

## Project overview

https://github.com/user-attachments/assets/dfcd8c64-90fe-43d2-bef9-cfd7308a3d50

This project targets an STM32F411CEU6 Black Pill board and uses STM32Cube HAL through PlatformIO's `stm32cube` framework. It reads the date and time from a DS1307 real-time clock and temperature and pressure from a BMP280 sensor, then displays the readings on a 128×64 SSD1306 OLED. The display is refreshed once per second.

## Hardware and connections

The DS1307 and SSD1306 share I2C1. The BMP280 uses SPI1 with a separate chip-select output:

| STM32F411 pin | Signal | Device |
| --- | --- | --- |
| PB6 | I2C1 SCL | DS1307 and SSD1306 |
| PB7 | I2C1 SDA | DS1307 and SSD1306 |
| PA5 | SPI1 SCK | BMP280 |
| PA6 | SPI1 MISO | BMP280 |
| PA7 | SPI1 MOSI | BMP280 |
| PA4 | Chip select (active low) | BMP280 |

The firmware uses the DS1307 I2C address `0x68` and the SSD1306 display address configured in `lib/ssd1306/ssd1306_conf.h`. Confirm the supply-voltage and pull-up requirements of your RTC, sensor, and display modules before connecting them.

## Behavior

- Initializes the SSD1306 and BMP280, then reads the RTC and sensor once per second.
- Displays the time, weekday/date, temperature in °C, and pressure in hPa.
- Reports a sensor read failure over the USB CDC serial interface.
- Includes `DS1307_SetTime()` in the application library, but the main application currently only reads the RTC. Set the RTC before running if it does not already have the correct time.

## Project structure

- `Core/Src/app/main_cpp.cpp` — sensor initialization, periodic reads, and OLED rendering
- `Core/Src/app/ds1307.cpp` and `Core/Inc/app/ds1307.h` — DS1307 time read/write functions
- `Core/Src/app/bmp280.cpp` and `Core/Inc/app/bmp280.h` — BMP280 SPI register access
- `lib/BoschBMP280/` — Bosch BMP280 driver
- `lib/ssd1306/` — SSD1306 display driver and configuration
- `USB_DEVICE/` — USB CDC device support
- `platformio.ini` — PlatformIO build configuration

## Build and upload

From this project directory, build the default `blackpill_f411ce` environment:

```sh
pio run
```

Upload to the board:

```sh
pio run -t upload
```

[Back to the main project README](../README.md)
