# STM32F411 PWM Sine-Wave Buzzer

## Project Overview

This project targets an STM32F411CEU6 and uses STM32Cube HAL through PlatformIO's `stm32cube` framework. TIM3 channel 1 generates PWM on PA6. TIM3 update interrupts step through a 16-entry sine-shaped duty table, producing a changing PWM duty cycle intended to synthesize musical notes for a buzzer.

The repository contains CubeMX-generated initialization and interrupt support alongside a C++ application that implements the audio and melody behavior. The project files identify PA6 as the buzzer signal output, but do not include a schematic or document the buzzer model, wiring, or any external driver circuitry.

## Hardware

- MCU configured by the project: STM32F411CEU6, UFQFPN48 package.
- PlatformIO environment selects the `blackpill_f411ce` board definition.
- PWM output: PA6 / TIM3_CH1.
- The application names its output functions `Buzzer_*`; physical buzzer wiring and electrical requirements cannot be determined from the repository.
- PH0 and PH1 are assigned to the HSE oscillator in the CubeMX configuration.

### TIM3 / PWM Configuration

CubeMX initializes TIM3 as an up-counter using its internal clock, with clock division 1 and auto-reload preload disabled. Channel 1 is PWM mode 1, active-high, with an initial compare/pulse value of zero. The CubeMX initial prescaler is 0 and the initial period (ARR) is 65535.

The application then changes the settings at runtime:

- `main_cpp()` starts PWM on `TIM_CHANNEL_1` and writes `PRESCALER = 97` to TIM3.
- `Buzzer_SetFrequency()` computes and writes a per-note ARR.
- Each update interrupt writes a new channel-1 compare value (CCR).

With the application prescaler, the timer counter frequency is:

```text
timer_counter_frequency = timer_clock / (PSC + 1)
                         = 98,000,000 / (97 + 1)
                         = 1,000,000 Hz
```

The runtime prescaler is therefore not the CubeMX initial prescaler. Likewise, ARR starts as 65535 in CubeMX but is recalculated for each nonzero note frequency by the application.

## Audio Synthesis

### Sine Lookup Table

`main_app.cpp` defines `SINE_SAMPLES = 16` and this duty-percent table:

```text
50, 69, 85, 96, 100, 96, 85, 69,
50, 31, 15, 4, 0, 4, 15, 31
```

These are 16 uniformly spaced points for one cycle of a sine-shaped waveform, represented as duty percentages from 0 to 100. This avoids evaluating a trigonometric function in the timer interrupt. Sixteen points correspond to 22.5 degrees of phase per step. The source does not document why 16 was selected; operationally, it provides a compact/coarse waveform table and means 16 duty updates are made per nominal audio cycle. More points would increase waveform resolution but also increase the interrupt/update rate proportionally for a fixed note.

### Note Generation

Notes are represented by integer frequencies in hertz, for example `NOTE_E4 = 330` and `NOTE_G4 = 392`. For a requested nonzero note, `Buzzer_SetFrequency()` computes the desired update rate as:

```text
sample_update_frequency = note_frequency x SINE_SAMPLES
```

The counter must overflow once for every table sample, so the timer period is calculated from the counter frequency:

```text
period_ticks = timer_counter_frequency / sample_update_frequency
ARR = period_ticks - 1
```

TIM3's update event rate is therefore approximately `sample_update_frequency`. The calculation uses integer division, so the actual rate is quantized:

```text
actual_update_frequency = timer_counter_frequency / (ARR + 1)
actual_waveform_frequency = actual_update_frequency / SINE_SAMPLES
```

For E4 (330 Hz), the requested update rate is 330 x 16 = 5280 Hz. At a 1 MHz counter rate, integer division gives 189 ticks, so ARR is 188. The actual update rate is about 5291.0 Hz and the resulting 16-step waveform repetition rate is about 330.7 Hz, rather than exactly 330 Hz.

For each table value, `Sound_UpdateSample()` calculates the compare value:

```text
period_ticks = ARR + 1
CCR = period_ticks x duty_percent / 100
```

The resulting CCR changes the PWM high-time fraction for that timer period. The PWM output remains a digital high/low signal; its changing duty-cycle average follows the sine-shaped table. The timer update interrupt advances `sample_index`, wraps it to zero at 16, and repeats the table. `HAL_TIM_PeriodElapsedCallback()` calls this update function for TIM3.

### Note Playback Control

`Sound_PlayNote(frequency, duration_ms)` configures the note rate, blocks the main thread in `HAL_Delay(duration_ms)`, then stops the note. TIM3 interrupts continue updating PWM while the main thread waits. `Buzzer_Stop()` disables the TIM3 update interrupt and writes CCR = 0; it does not stop the PWM counter/channel itself. `Sound_Pause(duration_ms)` stops the output and waits with `HAL_Delay()`.

### Melody / Playback

Jingle Bells

The melody runs continuously in `main_cpp()`:

1. E4 for 250 ms, pause 80 ms; E4 for 250 ms, pause 80 ms; E4 for 500 ms; pause 300 ms.
2. Repeat the same E4, E4, E4 phrase and timings.
3. Play E4, G4, C4, and D4 for 250 ms each, with 80 ms pauses between them; play E4 for 500 ms; pause 1000 ms.
4. Repeat from the first phrase.

The code also defines D4, F4, A4, B4, and C5 frequency constants. In the current melody, D4 is used, while F4, A4, B4, and C5 are not played.

## How It Works

The application uses TIM3's same auto-reload period both to set the PWM period and to produce update events. Each update event triggers an interrupt, and the callback changes CCR for the next sample. Thus the PWM period/update rate is the sample-update rate; the project does not configure a separate faster PWM carrier timer.

```text
98 MHz APB1 timer clock
    -> PSC = 97
    -> 1 MHz TIM3 counter
    -> note-specific ARR / update event
    -> TIM3 update IRQ and HAL_TIM_PeriodElapsedCallback
    -> next of 16 sine-table duty values
    -> CCR update for TIM3_CH1
    -> PWM duty cycle on PA6
    -> buzzer signal (physical connection not documented)
```

## Build

From the project root, build the configured PlatformIO environment:

```sh
pio run
```

## Flash / Upload

Upload using the configured ST-Link protocol:

```sh
pio run -t upload
```

The upload protocol is `stlink`; the configuration does not specify a device serial number or custom upload port.