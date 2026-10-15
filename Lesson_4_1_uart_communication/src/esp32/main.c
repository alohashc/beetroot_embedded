#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "iot_button.h"
#include "button_gpio.h"

#define UART_PORT UART_NUM_1
#define UART_BAUD_RATE 115200
#define UART_TX_PIN GPIO_NUM_17
#define UART_RX_PIN GPIO_NUM_18
#define UART_RX_BUFFER_SIZE 256
#define UART_TX_BUFFER_SIZE 0

#define BUTTON_IO_NUM GPIO_NUM_15
#define LED_PIN GPIO_NUM_16

#define LED_COMMAND "Change LED State"
#define SEND_DATA LED_COMMAND "\n"
#define RX_BUFFER_SIZE 64


static void button_single_click_event_cb(void *arg, void *data)
{
    printf("Button pressed, sending command to STM32...\n");
    uart_write_bytes(UART_PORT, SEND_DATA, strlen(SEND_DATA));
}

void app_main(void)
{
    // Initialize GPIO for LED
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    bool led_state = false;

    // Initialize button
    const button_config_t btn_cfg = {
        .long_press_time = 5000,
        .short_press_time = 200,
    };
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = BUTTON_IO_NUM,
        .active_level = 0,
        .disable_pull = false,

    };
    button_handle_t btn;
    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &btn);
    ret = iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, NULL, button_single_click_event_cb, NULL);
    ESP_ERROR_CHECK(ret);

    // Initialize UART
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT};

    uart_driver_install(UART_PORT, UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE, 0, NULL, 0);
    uart_param_config(UART_PORT, &config);
    uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    char rx_buffer[RX_BUFFER_SIZE] = {};
    uint8_t received_byte = 0;
    int16_t index = 0;

    while (1)
    {
        int len = uart_read_bytes(UART_PORT, &received_byte, 1, pdMS_TO_TICKS(100));

        if (len == 0)
        {
            continue;
        }

        if (received_byte == '\n')
        {
            rx_buffer[index] = '\0';
            printf("Received string: %s\n", rx_buffer);

            if (strcmp(rx_buffer, LED_COMMAND) == 0)
            {
                led_state = !led_state;
                gpio_set_level(LED_PIN, led_state);
            }

            index = 0;
        }
        else if (index < RX_BUFFER_SIZE - 1)
        {
            rx_buffer[index++] = (char)received_byte;
        }
    }
}