#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"

#include "driver/gpio.h"

#define ADC_CHANNEL ADC_CHANNEL_3
#define LED_PIN GPIO_NUM_18

#define SMA_SIZE 5

#define THRESHOLD_MV 2100
#define HYSTERESIS_MV 200

#define LOW_THRESHOLD_MV (THRESHOLD_MV - HYSTERESIS_MV)
#define HIGH_THRESHOLD_MV (THRESHOLD_MV + HYSTERESIS_MV)

int sma_calculate(int *buffer, int new_value)
{
    static int buffer_count = 0;
    static int buffer_index = 0;
    static int sum = 0;

    sum -= buffer[buffer_index];
    buffer[buffer_index] = new_value;
    sum += new_value;
    buffer_index = (buffer_index + 1) % SMA_SIZE;

    if (buffer_count < SMA_SIZE)
    {
        buffer_count++;
    }

    return sum / buffer_count;
}

extern "C" void app_main()
{
    esp_err_t status;
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t adc_unit_config = {};
    
    adc_unit_config.unit_id = ADC_UNIT_1;

    status = adc_oneshot_new_unit(
        &adc_unit_config,
        &adc1_handle);

    if (status != ESP_OK)
    {
        printf("ADC unit initialization failed: %d\n", status);
        return;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    status = adc_oneshot_config_channel(
        adc1_handle,
        ADC_CHANNEL,
        &chan_cfg);

    if (status != ESP_OK)
    {
        printf("ADC channel configuration failed: %d\n", status);
        return;
    }

    adc_cali_handle_t adc1_calibration_handle = nullptr;

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .chan = ADC_CHANNEL,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    status = adc_cali_create_scheme_curve_fitting(
        &cali_config,
        &adc1_calibration_handle);

    if (status != ESP_OK)
    {
        printf("ADC calibration initialization failed: %d\n", status);
        return;
    }

    gpio_config_t gpio_led_config = {
        .pin_bit_mask = 1ULL << LED_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    status = gpio_config(&gpio_led_config);

    if (status != ESP_OK)
    {
        printf("GPIO configuration failed: %d\n", status);
        return;
    }

    status = gpio_set_level(LED_PIN, 0);

    if (status != ESP_OK)
    {
        printf("Failed to set initial LED state: %d\n", status);
        return;
    }

    int sma_buffer[SMA_SIZE] = {0};
    bool led_state = false;

    while (1)
    {
        int adc_raw = 0;
        int adc_voltage = 0;

        status = adc_oneshot_read(
            adc1_handle,
            ADC_CHANNEL,
            &adc_raw);

        if (status == ESP_OK)
        {
            status = adc_cali_raw_to_voltage(
                adc1_calibration_handle,
                adc_raw,
                &adc_voltage);
        }

        if (status == ESP_OK)
        {
            int sma_voltage = sma_calculate(
                sma_buffer,
                adc_voltage);

            printf(
                "ADC raw: %d, voltage: %d mV, SMA: %d mV\n",
                adc_raw,
                adc_voltage,
                sma_voltage);

            if (sma_voltage < LOW_THRESHOLD_MV)
            {
                led_state = true;
            }
            else if (sma_voltage > HIGH_THRESHOLD_MV)
            {
                led_state = false;
            }

            status = gpio_set_level(
                LED_PIN,
                led_state);

            if (status != ESP_OK)
            {
                printf("LED update failed: %d\n", status);
            }
        }
        else
        {
            printf("ADC processing failed: %d\n", status);
        }

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}