#include <stdio.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pwm.h"
#include "servo.h"
#include "adc.h"

#define SERVO_GPIO GPIO_NUM_15
#define ADC_IN_CHANNEL_1 ADC_CHANNEL_3 // GPIO4

#define POT_MAX_ANGLE 300
#define ADC_BIT 12
#define ADC_MAX ((1U << ADC_BIT) - 1U)

static const char *TAG = "servo:main";

extern "C" void app_main(void)
{
    adc_oneshot_ctx_t adc_ctx;
    adc_oneshot_init(&adc_ctx,
                     ADC_UNIT_1,
                     ADC_RTC_CLK_SRC_DEFAULT,
                     ADC_ULP_MODE_DISABLE);

    adc_oneshot_config(&adc_ctx,
                       ADC_IN_CHANNEL_1,
                       ADC_ATTEN_DB_12,
                       ADC_BITWIDTH_DEFAULT,
                       true);

    pwm_t servo_pwm = {};
    pwm_config_t servo_cfg = {
        .gpio = SERVO_GPIO,
        .channel = LEDC_CHANNEL_0,
        .timer = LEDC_TIMER_0,
        .frequency_hz = SERVO_FREQUENCY_HZ,
        .resolution = LEDC_TIMER_14_BIT,
        .duty = 0,
        .inverted = false};

    esp_err_t ret = pwm_init(&servo_pwm, &servo_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "pwm_init failed: %s", esp_err_to_name(ret));
    }

    servo_t servo = {};
    ret = servo_init(&servo, &servo_pwm);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "servo_init failed: %s", esp_err_to_name(ret));
        pwm_deinit(&servo_pwm);
    }

    while (1)
    {
        int raw = 0;
        adc_oneshot_read_raw(&adc_ctx, ADC_IN_CHANNEL_1, &raw);

        uint16_t angle = ((uint32_t)raw * POT_MAX_ANGLE) / ADC_MAX;

        if (angle > 180)
        {
            angle = 180;
        }

        ret = servo_set_angle(&servo, angle);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "servo_set_angle failed: %s", esp_err_to_name(ret));
        }

        printf("ADC raw: %d, angle: %u degrees\n", raw, angle);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}