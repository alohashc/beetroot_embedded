#include "encoder.h"
#include "servo.h"
#include "sine.h"
#include "esp_log.h"
#include "pwm_config.h"

#define ENCODER_A_INPUT      GPIO_NUM_15
#define ENCODER_B_INPUT      GPIO_NUM_16
#define ENCODER_BUTTON_INPUT GPIO_NUM_17
#define ENCODER_DEBOUNCE_NS  1000

#define ATTEMPT_PIN_1   GPIO_NUM_5
#define ATTEMPT_PIN_2   GPIO_NUM_6
#define ATTEMPT_PIN_3   GPIO_NUM_7
#define LOCK_PIN_1      GPIO_NUM_1
#define SUCCESS_PIN_2   GPIO_NUM_2

static const char *TAG = "Safe::init";

servo_t servo_create(void)
{
    servo_t servo = {};
    esp_err_t ret = servo_init(&servo, &servo_pwm);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "servo_init failed: %s", esp_err_to_name(ret));
        pwm_deinit(&servo_pwm);
    }

    return servo;
}

sine_t tone_create(void)
{
    sine_t tone = {};
    esp_err_t ret = sine_init(&tone, &buzzer_pwm, 150);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "sine_init failed: %s", esp_err_to_name(ret));
        pwm_deinit(&buzzer_pwm);
    }

    return tone;
}

encoder_ctx_t encoder_create(void)
{
    encoder_ctx_t encoder;

    esp_err_t ret = encoder_init(
        &encoder,
        ENCODER_A_INPUT,
        ENCODER_B_INPUT,
        ENCODER_BUTTON_INPUT,
        ENCODER_DEBOUNCE_NS
    );
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "encoder_init failed: %s", esp_err_to_name(ret));
    }

    return encoder;
}

void gpio_leds_init()
{
    gpio_config_t config = {};

    config.pin_bit_mask =
        (1ULL << ATTEMPT_PIN_1) |
        (1ULL << ATTEMPT_PIN_2) |
        (1ULL << ATTEMPT_PIN_3) |
        (1ULL << LOCK_PIN_1) |
        (1ULL << SUCCESS_PIN_2);

    config.mode = GPIO_MODE_OUTPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&config));
}

void set_gpio_attempts(uint8_t attempts)
{
    gpio_set_level(ATTEMPT_PIN_1, (attempts >= 1) ? 1 : 0);
    gpio_set_level(ATTEMPT_PIN_2, (attempts >= 2) ? 1 : 0);
    gpio_set_level(ATTEMPT_PIN_3, (attempts >= 3) ? 1 : 0);
}

void set_gpio_locked(bool locked)
{
    gpio_set_level(LOCK_PIN_1, locked ? 1 : 0);
}

void set_gpio_success(bool success)
{
    gpio_set_level(SUCCESS_PIN_2, success ? 1 : 0);
}
