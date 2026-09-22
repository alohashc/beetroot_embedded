#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "adc.h"
#include "pwm.h"


#define ADC_CHANNEL ADC_CHANNEL_3 // GPIO4
#define LED_OUT GPIO_NUM_18
#define MOTOR_OUT GPIO_NUM_15

constexpr uint32_t PWM_MAX = (1U << LEDC_TIMER_8_BIT) - 1U;
constexpr uint8_t ADC_BIT = 12;
constexpr uint32_t ADC_RAW_MAX = (1U << ADC_BIT) - 1U;
constexpr uint32_t LED_FREQUENCY_HZ = 1000;
constexpr uint32_t MOTOR_FREQUENCY_HZ = 20000;


extern "C" void app_main(void)
{
    adc_oneshot_ctx_t adc_ctx;
    adc_oneshot_init(&adc_ctx, ADC_UNIT_1, ADC_RTC_CLK_SRC_DEFAULT, ADC_ULP_MODE_DISABLE);

    adc_oneshot_config(&adc_ctx, ADC_CHANNEL, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT, true);

    pwm_t led_pwm = {};
    pwm_config_t led_cfg = {
        .gpio = LED_OUT,
        .channel = LEDC_CHANNEL_0,
        .timer = LEDC_TIMER_0,
        .frequency_hz = LED_FREQUENCY_HZ,
        .resolution = LEDC_TIMER_8_BIT,
        .duty = 0,
        .inverted = false,
    };

    pwm_t motor_pwm = {};
    pwm_config_t motor_cfg = {
        .gpio = MOTOR_OUT,
        .channel = LEDC_CHANNEL_1,
        .timer = LEDC_TIMER_1,
        .frequency_hz = MOTOR_FREQUENCY_HZ,
        .resolution = LEDC_TIMER_8_BIT,
        .duty = 0,
        .inverted = false,
    };

    ESP_ERROR_CHECK(pwm_init(&led_pwm, &led_cfg));
    ESP_ERROR_CHECK(pwm_init(&motor_pwm, &motor_cfg));

    while (1)
    {
        int adc_raw = 0;

        adc_oneshot_read_raw(&adc_ctx, ADC_CHANNEL, &adc_raw);

        float percent = adc_raw * 100.0f / ADC_RAW_MAX;

        ESP_ERROR_CHECK(pwm_set_percent(&led_pwm, (uint8_t)percent));
        ESP_ERROR_CHECK(pwm_set_percent(&motor_pwm, (uint8_t)percent));
        
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}