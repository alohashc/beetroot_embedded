#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "adc.h"
#include "cmath"

static const char *TAG = "LESSON_3_2";

#define ADC_CHANNEL ADC_CHANNEL_3 // GPIO4

constexpr uint32_t ADC_BITS = 12;
constexpr uint32_t ADC_MAX = (1U << ADC_BITS) - 1U; // 4095

constexpr float VREF_MV = 1100.0f;

extern "C" void app_main(void)
{
    adc_oneshot_ctx_t adc_ctx;
    adc_oneshot_init(&adc_ctx, ADC_UNIT_1, ADC_RTC_CLK_SRC_DEFAULT, ADC_ULP_MODE_DISABLE);

    adc_oneshot_config(&adc_ctx, ADC_CHANNEL, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT, true);

    while (1)
    {
        int adc_raw = 0;
        int adc_cali_voltage = 0;

        adc_oneshot_read_raw(&adc_ctx, ADC_CHANNEL, &adc_raw);
        adc_oneshot_read_voltage(&adc_ctx, ADC_CHANNEL, &adc_cali_voltage);

        float adc_voltage = ((float)adc_raw / ADC_MAX) * VREF_MV * (float)ADC_ATTEN_DB_12;
        float error = fabsf(adc_voltage - (float)adc_cali_voltage) / (float)adc_cali_voltage * 100.0f;

        ESP_LOGI(TAG,
         "RAW: %d | U_manual: %.1f mV | U_cali: %d mV | Error: %.2f%%",
         adc_raw, adc_voltage, adc_cali_voltage, error);
        
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}