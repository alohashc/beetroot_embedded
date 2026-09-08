# Lesson_2_5 — Timer-driven Motor Control (Beetroot Embedded)

This project demonstrates controlling a motor on an ESP32-S3 dev board using two hardware timers: one timer schedules when the motor starts, the other controls how long it runs.


https://github.com/user-attachments/assets/28287216-0d28-408d-84dd-58e3b4c17fa3


## What this project demonstrates
- Timer-driven motor control using `hw_timer_t` interrupts.
- Use of the task watchdog (`esp_task_wdt`) to keep the main loop alive.
- Simple status LEDs for ON/OFF and serial status reporting.

Example serial output:

```
Motor is OFF. Time until start: 9 seconds
Motor is OFF. Time until start: 8 seconds
...
Motor is ON
Motor is ON. Time until stop: 4 seconds
...
Motor is OFF
```

## Build / Upload / Monitor
From the project root:

```bash
pio run
pio run -t upload
pio device monitor
```

Or run upload + monitor together:

```bash
pio run -t upload -t monitor
```

Back to main: [Beetroot Embedded README](../README.md)
