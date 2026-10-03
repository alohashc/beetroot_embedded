# Mini Project 3 - Safe

This project is a compact ESP32-based safe control system built around a rotary encoder, a servo actuator, status LEDs, and a buzzer. The device accepts a 4-digit code, validates it, and unlocks a simulated safe door on success.

## Project goal

The safe is designed to demonstrate embedded control patterns for:

- rotary encoder input handling
- digital state transitions and validation logic
- PWM-based servo control
- PWM-driven buzzer feedback
- GPIO status indicators for attempts and lock state

The program expects the code `2164` and tracks failed attempts up to a 3-attempt lockout threshold.

## Hardware overview

The project uses the following peripherals:

- ESP32 development board
- Rotary encoder A/B/button inputs
- SG90-style servo motor for the lock mechanism
- Piezo buzzer for success and error tones
- Status LEDs for attempts, lock state, and success indication

Key GPIO mappings:

- Encoder A: GPIO 15
- Encoder B: GPIO 16
- Encoder button: GPIO 17
- Attempt LEDs: GPIO 5, 6, 7
- Lock LED: GPIO 1
- Success LED: GPIO 2
- Buzzer: GPIO 18
- Servo: GPIO 21

## Safe behavior

1. The system starts in a waiting state.
2. Rotating the encoder moves through the digit selection logic.
3. The button can be used to reset or start a new code entry sequence.
4. After four digits are captured, the code is validated.
5. A correct code triggers a success tone, unlocks the servo, and resets the safe.
6. A wrong code increases the attempt counter and triggers an error tone.
7. After the configured number of failed attempts, the safe enters a locked state until the board is reset.

## Build and run

From this project folder, run:

```bash
pio run
pio run --target upload
pio run --target monitor
```

## Files

- [src/main.cpp](src/main.cpp) — state machine and safe logic
- [src/init.cpp](src/init.cpp) — GPIO, servo, encoder, and buzzer initialization
- [src/init.h](src/init.h) — shared declarations
- [src/pwm_config.cpp](src/pwm_config.cpp) - Defintion for pwm's
- [platformio.ini](platformio.ini) — PlatformIO configuration

## Notes

This project is part of the Beetroot embedded lessons and demonstrates practical firmware control for a user-input safety device using ESP-IDF and PlatformIO.
