#include "app/app_main.h"
#include "main.h"
#include <cstdio>

extern "C" ADC_HandleTypeDef hadc1;
extern "C" UART_HandleTypeDef huart1;

static constexpr uint32_t ADC_SAMPLES = 16;

static uint16_t adcBuffer[ADC_SAMPLES];
static char uartBuffer[128];

static volatile bool adcReady = false;
static volatile bool uartBusy = false;
static volatile bool uartDone = false;

extern "C" void HAL_ADC_ConvCpltCallback(
    ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        adcReady = true;
    }
}

extern "C" void HAL_UART_TxCpltCallback(
    UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        uartBusy = false;
        uartDone = true;
    }
}

static uint16_t calculateAverage(
    const uint16_t *samples,
    size_t count)
{
    if (samples == nullptr || count == 0)
    {
        return 0;
    }

    uint32_t sum = 0;

    for (size_t i = 0; i < count; i++)
    {
        sum += samples[i];
    }

    return static_cast<uint16_t>(sum / count);
}

void app_main(void)
{
    if (HAL_ADC_Start_DMA(
            &hadc1,
            reinterpret_cast<uint32_t *>(adcBuffer),
            ADC_SAMPLES) != HAL_OK)
    {
        Error_Handler();
    }

    while (1)
    {
        if (adcReady && !uartBusy)
        {
            adcReady = false;

            size_t offset = 0;

            uint16_t average = calculateAverage(
                adcBuffer,
                ADC_SAMPLES);

            int written = snprintf(
                uartBuffer,
                sizeof(uartBuffer),
                "ADC AVG: %u\r\n",
                static_cast<unsigned>(average));

            if (written < 0 ||
                static_cast<size_t>(written) >=
                    sizeof(uartBuffer) - offset)
            {
                Error_Handler();
            }

            offset += static_cast<size_t>(written);

            uartBusy = true;

            if (HAL_UART_Transmit_DMA(
                    &huart1,
                    reinterpret_cast<uint8_t *>(uartBuffer),
                    static_cast<uint16_t>(offset)) != HAL_OK)
            {
                uartBusy = false;
                Error_Handler();
            }
        }

        if (uartDone)
        {
            uartDone = false;

            if (HAL_ADC_Start_DMA(
                    &hadc1,
                    reinterpret_cast<uint32_t *>(adcBuffer),
                    ADC_SAMPLES) != HAL_OK)
            {
                Error_Handler();
            }
        }
    }
}
