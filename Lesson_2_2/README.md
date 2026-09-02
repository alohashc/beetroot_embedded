# Lesson 2.2 — Two Embedded Tasks (Relay timing / Software PWM motor)

## Overview
This sketch contains two selectable tasks controlled by compile-time defines:

- `TASK_1` — Relay activation time measurement using an interrupt on a contact input.
<img width="591" height="227" alt="Screenshot 2026-08-29 at 19 47 40" src="https://github.com/user-attachments/assets/d6a74b71-aa16-493b-b79d-78faa49380b1" />

https://github.com/user-attachments/assets/149ce9e9-a472-4828-9ccf-5f1e1513da1c

- `TASK_2` — Motor speed control using a potentiometer and a software PWM implementation.

https://github.com/user-attachments/assets/13bf5299-efdb-4665-ad01-578713485e5d



Only one task must be selected at build time (see Build & Run).

## Common code patterns used
- `constexpr` for pin numbers and timing constants
- `volatile` variables for ISR-to-main signalling
- Non-blocking control using `millis()` / `micros()`
- Minimal ISR bodies (set flags / timestamps only)

## TASK 1 — Relay activation time measurement
Purpose: measure the time from turning a relay on to a contact closure event, repeat N times and print the average.

Key details:
- Relay output pin: `RELAY_PIN = 18`
- Contact input pin: `CONTACT_PIN = 15` (configured as `INPUT_PULLUP`)
- Number of runs: `RUN_NUMBER = 10`
- ISR: `onRelayContact()` captures `millis()` into a `volatile unsigned long contactMillis` and sets a `volatile bool contactTriggered` flag.
- Main loop: toggles the relay on, waits for the contact event, logs each timing and then computes the average.
- Uses `delay()` only in `setup()` for startup pause; main loop is non-blocking.

Good for learning interrupt-driven measurement and safe ISR design.

## TASK 2 — Motor speed control (software PWM)
Purpose: generate a software PWM on a GPIO using `micros()` and change duty cycle with an ADC-connected potentiometer.

Key details:
- Motor output pin: `MOTOR_PIN = 18`
- Potentiometer ADC pin: `POT_PIN = 4`
- PWM period: `PWM_PERIOD_MICROS = 1000` (1 ms period)
- ADC resolution constant: `ADC_MAX = 4095`
- Algorithm: read ADC, compute `highTimeMicros = PWM_PERIOD_MICROS * adc / ADC_MAX`, then toggle `digitalWrite()` using `micros()` timestamps for accurate timing.
- Handles 0% and 100% duty cycle as shortcuts.

Good for learning software PWM, using `micros()` for high-resolution timing, and mapping ADC values.

## Build & Run
Select the desired task at build time by defining `TASK_1` or `TASK_2`.

Examples (from project folder):

```bash
# Build TASK_1
pio run -D TASK_1
# Upload TASK_1
pio run -D TASK_1 -t upload
# Monitor serial
pio device monitor
```

Or replace `TASK_1` with `TASK_2` for the motor PWM task.

If you prefer, add the define to `platformio.ini` under an environment's `build_flags`:

```
build_flags = -DTASK_1
```

## Notes and safety
- ISR functions are minimal and marked with `IRAM_ATTR` where present to ensure safe ISR placement.
- `volatile` is used for variables shared between ISR and `loop()`.
- The code avoids dynamic allocation and STL.
- TASK_1 will stop after `RUN_NUMBER` samples and print the average.

## Link back
[Back to main project README](../README.md)
