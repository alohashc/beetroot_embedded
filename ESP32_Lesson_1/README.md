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
## Photos
### CASE 1
<img width="960" height="1280" alt="photo_2026-08-07 17 56 42" src="https://github.com/user-attachments/assets/9bc72ce0-c771-4c70-a96d-fa6aafac72ad" />
<img width="960" height="1280" alt="photo_2026-08-07 17 56 47" src="https://github.com/user-attachments/assets/aba8f2d0-999e-4491-91a4-b62b436019bc" />


### CASE 2
<img width="960" height="1280" alt="photo_2026-08-07 17 56 39" src="https://github.com/user-attachments/assets/b3b5c8de-ff5d-4188-ac89-81af413f93d1" />
<img width="960" height="1280" alt="photo_2026-08-07 17 56 29" src="https://github.com/user-attachments/assets/463d4ce9-b295-485c-9f02-aef2b4e3b595" />


[Back to main project README](../README.md)
