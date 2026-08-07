# ESP32 Lesson 1

This Lesson 1 project uses PlatformIO to build the ESP32-S3 example with two lesson-specific build cases.

## Lesson 1 build cases

- `case_1`: simple police blink using two GPIO connections, built with `-DCASE_POLICE_BLINK_SIMPLE`
- `case_2`: police blink using only one GPIO connection, built with `-DCASE_POLICE_BLINK_ONE_GPIO`

## Build commands

Run case 1:

```bash
pio run -e case_1
```

Run case 2:

```bash
pio run -e case_2
```

## Upload to board

Use the same environment name when uploading:

```bash
pio run -e case_1 -t upload
```

or

```bash
pio run -e case_2 -t upload
```

## Serial monitor

Start the serial monitor after upload:

```bash
pio device monitor
```

---

[Back to main project README](../README.md)
