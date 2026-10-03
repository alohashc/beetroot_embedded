#ifndef INIT_H
#define INIT_H

#include "encoder.h"
#include "servo.h"
#include "sine.h"

typedef enum {
    NONE,
    RIGHT,
    LEFT
} direction_e;

typedef struct {
    int32_t last_pulses;
    int32_t pulses;
    bool button_pressed;
    bool last_button_pressed;

} encoder_state_t;

servo_t servo_create(void);
sine_t tone_create(void);
encoder_ctx_t encoder_create(void);
void gpio_leds_init(void);
void set_gpio_attempts(uint8_t attempts);
void set_gpio_locked(bool locked);
void set_gpio_success(bool success);

#endif
