# Module 4 Mini Project: Logger

This project demonstrates a small persistent logger for an STM32F411
Black Pill. The logger is separated from its storage hardware through a
`LoggerDevice_t` adapter, so the logger core can work with different devices
that provide read and write operations.

The current application uses an AT24C32 I2C EEPROM as its storage device.

## Design

- **Storage adapter** — `lib/logger/logger_device.h` defines
  `LoggerDevice_t`: a device context, read and write callbacks, and storage
  capacity. A hardware-specific adapter implements those callbacks.
- **Logger core** — `lib/logger/logger.h` exposes the logger API. Initialize a
  `Logger_t` with a configured `LoggerDevice_t`; the core uses the callbacks
  rather than depending directly on the EEPROM driver.
- **EEPROM backend** — `lib/at24c32/` contains the current AT24C32 I2C driver.
  `Core/Src/app/app_main.cpp` connects it to the logger through read and write
  callbacks.
- **On-board test program** — `app_main()` clears storage, writes and reads
  records, checks the count and first/last record numbers, reinitializes the
  logger to demonstrate state recovery, and writes enough records to exercise
  circular overwrite and log-number rollover.

Each log record occupies 32 bytes and is formatted as `#<number> <message>`.
Numbers run from 0 through 999 and then wrap to 0. When storage fills, new
records overwrite the oldest ones. With the current 4096-byte AT24C32, the
logger can retain up to 128 records. Messages that do not fit in a record are
truncated.

## Hardware and output

- STM32F411 Black Pill board
- AT24C32 EEPROM connected to the board's configured I2C1 bus
- USB connection to view the application's `printf` output through USB CDC
- ST-LINK for firmware upload

The EEPROM address is configured as `0x50` in `lib/at24c32/at24c32.h`.

## Build and upload

From this directory, use PlatformIO:

```sh
pio run
pio run -t upload
```

Connect a USB CDC serial terminal to the board to view the logger demonstration
output. The configured PlatformIO environment is `blackpill_f411ce`.

## Main logger API

- `LoggerDevice_Init()` configures the storage callbacks and capacity.
- `Logger_Init()` initializes a logger and scans storage to recover its state.
- `Logger_Write()` appends a record; `Logger_Read()` reads a record by number.
- `Logger_GetCount()`, `Logger_GetFirstNumber()`, and
  `Logger_GetLastNumber()` report the current retained records.
- `Logger_Clear()` clears the records in storage.

The on-board demonstration is implemented in
[`Core/Src/app/app_main.cpp`](Core/Src/app/app_main.cpp); the reusable logger
interface and device adapter are in [`lib/logger/`](lib/logger/).
