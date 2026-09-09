# Traffic Lights

Simple PlatformIO project for an ESP32-based traffic light controller.

## Overview

This project implements a basic traffic light sequence using hardware timers on an ESP32. The implementation lives in `src/main.cpp`.

## Features

- Configurable on/blink intervals in `src/main.cpp`
- Uses hardware timers (`hw_timer_t`) for accurate timing

## Traffic Light Phases

The behaviour below matches the timings and modes defined in `src/main.cpp`.

1. Phase "Movement allowed" (Green):
	- Only the green LED is lit for 5 seconds (`greenLedOn.onInterval = 5000`).
	- The firmware uses non-blocking timers and state updates (no `delay()`), driven by hardware timers.

2. Phase "Attention — prepare to stop" (Blinking Green):
	- The green LED toggles every 500 ms (`greenLedBlink.blinkInterval = 500`) for a total of 3 seconds (`greenLedBlink.onInterval = 3000`).
	- The code tracks both the overall blink duration and the per-flash interval without blocking.

3. Phase "Prohibition" (Yellow):
	- The green LED is off and a steady yellow LED is on for 2 seconds (`yellowLedOn.onInterval = 2000`).

4. Phase "Stop" (Red):
	- Only the red LED is lit for 5 seconds (`redLedOn.onInterval = 5000`).

5. Phase "Prepare to start" (Red + Yellow):
	- The red LED remains on and the yellow LED is added for 2 seconds (`redLedReady.onInterval = 2000`, `yellowLedReady.onInterval = 2000`).
	- This follows Ukrainian traffic rules where red+yellow signals imminent green.

6. Cycle closes:
	- Red and yellow turn off and green turns on again.


## Build & Upload

Use PlatformIO (in the project root) to build and upload:

```bash
pio run --target upload
```

You can also build only with `pio run` or upload+monitor with `pio run --target upload --target monitor`.

## Tuning

Open `src/main.cpp` and adjust the `t_Led_Params` intervals near the top to change durations and blink rates.

## Files

- `src/main.cpp` — main firmware
- `platformio.ini` — PlatformIO project configuration

## License

This project is unlicensed; add a license file if you intend to redistribute.
