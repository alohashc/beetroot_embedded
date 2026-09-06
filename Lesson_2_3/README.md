# Lesson 2.3 — Three non-blocking LED blinkers (millis-based) (C-Style)

## Overview
This lesson demonstrates non-blocking LED control on an ESP32 using `millis()` and a simple data structure per LED. The sketch runs three independent blinkers with different periods without using `delay()`.

## What the firmware does
- Controls three LEDs on GPIO pins: 15, 16 and 18.
- Each LED blinks at its own interval:
  - LED1: 200 ms
  - LED2: 500 ms
  - LED3: 1000 ms
- Uses a `t_led` struct to hold per-LED configuration and runtime state: `pin`, `interval`, `previousMillis`, and `state`.
- `toggleLed()` is called each loop iteration for each LED; it updates state when the interval elapsed.

## Key concepts demonstrated
- Non-blocking timing using `millis()` (superloop architecture)
- Encapsulation of per-peripheral state in a struct
- Avoiding `delay()` for concurrent behavior
- Use of `constexpr` for compile-time constants

## Pins and parameters
- `LED1_PIN = 15`, interval 200 ms
- `LED2_PIN = 16`, interval 500 ms
- `LED3_PIN = 18`, interval 1000 ms

## Build & Run
From the project folder:

```bash
pio run
pio run --target upload
pio run --target monitor
```

## Link back
[Back to main project README](../README.md)
