#include "encoder.h"
#include "servo.h"
#include "sine.h"
#include "esp_log.h"

static const char *TAG = "Safe::config";

pwm_config_t servo_cfg = {
    .gpio = GPIO_NUM_21,
    .channel = LEDC_CHANNEL_0,
    .timer = LEDC_TIMER_0,
    .frequency_hz = SERVO_FREQUENCY_HZ,
    .resolution = LEDC_TIMER_14_BIT,
    .duty = 0,
    .inverted = false};

pwm_config_t buzzer_cfg = {
    .gpio = GPIO_NUM_18,
    .channel = LEDC_CHANNEL_1,
    .timer = LEDC_TIMER_1,
    .frequency_hz = 10000,
    .resolution = LEDC_TIMER_12_BIT,
    .duty = 0,
    .inverted = false
};

pwm_t pwm_create(const pwm_config_t *cfg)
{
    pwm_t pwm = {};
    esp_err_t ret = pwm_init(&pwm, cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "pwm_init failed: %s", esp_err_to_name(ret));
    }
    return pwm;
}

pwm_t servo_pwm = pwm_create(&servo_cfg);
pwm_t buzzer_pwm = pwm_create(&buzzer_cfg);
