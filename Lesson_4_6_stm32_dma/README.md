# Lesson 4.6: STM32 ADC with DMA and UART Output

## Project overview

This project targets an STM32F411CEU6 Black Pill board and uses STM32Cube HAL through PlatformIO's `stm32cube` framework. It samples an analog input on ADC1, transfers the results using DMA, calculates an average, and sends the value over USART1.

The firmware captures 16 ADC samples into a buffer, computes the arithmetic mean, and prints the result as a serial message such as `ADC AVG: 2048` at 115200 baud. This demonstrates the STM32 DMA pattern for ADC-to-memory transfers and memory-to-peripheral UART transmission without blocking the CPU.

## Hardware and connections

The project uses ADC1 on PA0 and USART1 on PA9/PA10:

| STM32F411 pin | Signal | Device |
| --- | --- | --- |
| PA0 | ADC1_IN0 | Analog source (for example a potentiometer or sensor output) |
| PA9 | USART1 TX | USB-to-TTL serial adapter RX |
| PA10 | USART1 RX | USB-to-TTL serial adapter TX |

A simple voltage divider or potentiometer connected to PA0 provides the analog input. Connect the UART TX/RX pins to a serial adapter or USB-UART converter to view the measured average on a terminal.

## Behavior

- Configures ADC1 in continuous mode and uses DMA to store 16 samples in memory.
- Waits for the DMA transfer-complete callback, computes the average value, and formats it into a UART message.
- Sends the UART payload with `HAL_UART_Transmit_DMA()` so the CPU can continue processing other work while the transfer completes.
- Restarts the ADC DMA transfer after each UART transmission.

## Project structure

- `Core/Src/app/app_main.cpp` — ADC sampling loop, averaging logic, and UART output scheduling
- `Core/Src/adc.c` — ADC1 configuration and PA0 analog input setup
- `Core/Src/dma.c` — DMA controller initialization for ADC and UART transfers
- `Core/Src/usart.c` — USART1 serial configuration at 115200 baud
- `Core/Inc/` — generated HAL headers and application declarations
- `platformio.ini` — PlatformIO build configuration for the Black Pill board

## Build and upload

From this project directory, build the default `blackpill_f411ce` environment:

```sh
pio run
```

Upload to the board:

```sh
pio run -t upload
```

Open a serial monitor to read the ADC average values:

```sh
pio device monitor -b 115200
```

[Back to the main project README](../README.md)
