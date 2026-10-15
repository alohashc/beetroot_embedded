#include <cstdint>
#include <cstring>
#include <stdio.h>

#include "app_main.h"
#include "main.h"

#define RX_BUFFER_SIZE 64
#define LED_COMMAND "Change LED State"
#define SEND_DATA LED_COMMAND "\n"

extern "C" UART_HandleTypeDef huart1;

extern "C" void app_main(void)
{
    char rx_buffer[RX_BUFFER_SIZE] = {};
    uint8_t received_byte = 0;
    int16_t index = 0;

    constexpr uint32_t DEBOUNCE_MS = 30;

    // Variables for button debouncing
    GPIO_PinState last_reading = GPIO_PIN_SET;
    GPIO_PinState stable_state = GPIO_PIN_SET;
    uint32_t last_change_time = HAL_GetTick();

    while (true)
    {
        HAL_StatusTypeDef status = HAL_UART_Receive(
            &huart1,
            &received_byte,
            1,
            10);

        if (status == HAL_OK)
        {
            if (received_byte == '\n')
            {
                rx_buffer[index] = '\0';
                printf("Received string: %s\n", rx_buffer);

                if (strcmp(rx_buffer, LED_COMMAND) == 0)
                {
                    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
                }

                index = 0;
            }
            else if (index < RX_BUFFER_SIZE - 1)
            {
                rx_buffer[index++] = static_cast<char>(received_byte);
            }
        }

        GPIO_PinState reading = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);

        if (reading != last_reading)
        {
            last_change_time = HAL_GetTick();
        }

        if ((HAL_GetTick() - last_change_time) >= DEBOUNCE_MS)
        {
            if (reading != stable_state)
            {
                stable_state = reading;

                if (stable_state == GPIO_PIN_RESET)
                {
                    printf("Button pressed, sending command to ESP32...\n");
                    HAL_UART_Transmit(
                        &huart1,
                        reinterpret_cast<const uint8_t *>(SEND_DATA),
                        sizeof(SEND_DATA) - 1,
                        100);
                }
            }
        }

        last_reading = reading;
    }
}