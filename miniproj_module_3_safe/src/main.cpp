#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "init.h"

#define ATTEMPT_MAX 3
#define CODE_LENGTH 4
#define PULSES_PER_TICK 4

constexpr uint8_t valid_code[CODE_LENGTH] = {2, 1, 6, 4};

static const char *TAG = "Safe::main";

typedef enum
{
    SAFE_STATE_WAITING,
    SAFE_STATE_ENTERING,
    SAFE_STATE_VALIDATING,
    SAFE_STATE_LOCKED

} safe_state_e;

typedef struct
{
    uint8_t input_code[CODE_LENGTH];
    uint8_t code_index;
    uint8_t current_digit;
    uint8_t attempts;
    direction_e last_direction;
    safe_state_e state;

} safe_state_t;

static void encoder_get_values(const encoder_ctx_t *encoder, encoder_state_t *state)
{
    ESP_ERROR_CHECK(
        encoder_get_pulses(encoder, &state->pulses));

    ESP_ERROR_CHECK(
        encoder_get_button(encoder, &state->button_pressed));
}

static void play_sound_error(sine_t *tone)
{
    printf("Playing error sound...\n");
    sine_set_frequency(tone, 150);
    sine_start(tone);
    vTaskDelay(pdMS_TO_TICKS(150));
    sine_stop(tone);

    sine_set_frequency(tone, 100);
    sine_start(tone);
    vTaskDelay(pdMS_TO_TICKS(250));
    sine_stop(tone);
}

static void play_sound_success(sine_t *tone)
{
    printf("Playing success sound...\n");
    sine_set_frequency(tone, 1047);
    sine_start(tone);
    vTaskDelay(pdMS_TO_TICKS(150));
    sine_stop(tone);

    sine_set_frequency(tone, 1318);
    sine_start(tone);
    vTaskDelay(pdMS_TO_TICKS(100));
    sine_stop(tone);

    sine_set_frequency(tone, 1568);
    sine_start(tone);
    vTaskDelay(pdMS_TO_TICKS(100));
    sine_stop(tone);
}

static bool is_code_valid(const uint8_t *input_code)
{
    for (uint8_t i = 0; i < CODE_LENGTH; ++i)
    {
        if (input_code[i] != valid_code[i])
        {
            return false;
        }
    }

    return true;
}

static void reset_input(
    safe_state_t *safe_state,
    int32_t *pulse_counter)
{
    for (uint8_t i = 0; i < CODE_LENGTH; ++i)
    {
        safe_state->input_code[i] = 0;
    }

    safe_state->code_index = 0;
    safe_state->current_digit = 0;
    safe_state->last_direction = direction_e::NONE;
    *pulse_counter = 0;

    ESP_LOGI(
        TAG,
        "Reset. Attempt %d/%d",
        safe_state->attempts,
        ATTEMPT_MAX);

    ESP_LOGI(
        TAG,
        "INPUT: %d",
        safe_state->current_digit);
}

extern "C" void app_main(void)
{
    encoder_ctx_t encoder = encoder_create();
    servo_t servo = servo_create();
    sine_t tone = tone_create();

    gpio_leds_init();

    encoder_state_t encoder_state = {};
    encoder_get_values(&encoder, &encoder_state);
    encoder_state.last_pulses = 0;
    encoder_state.last_button_pressed = false;

    safe_state_t safe_state = {};
    safe_state.current_digit = 0;
    safe_state.state = SAFE_STATE_WAITING;
    vTaskDelay(pdMS_TO_TICKS(3000));

    ESP_LOGI(
        TAG,
        "INPUT: %d",
        safe_state.current_digit);

    while (1)
    {
        static int32_t pulse_counter = 0;
        direction_e tick_direction = direction_e::NONE;

        encoder_get_values(&encoder, &encoder_state);

        // Detect button click and pulse delta
        // Encoder makes 4 pulses per tick.
        bool button_clicked = encoder_state.button_pressed && !encoder_state.last_button_pressed;
        int32_t pulse_delta = encoder_state.pulses - encoder_state.last_pulses;

        pulse_counter += pulse_delta;

        // Determine the direction of the tick based on pulse count
        if (pulse_counter >= PULSES_PER_TICK)
        {
            tick_direction = direction_e::RIGHT;
            pulse_counter -= PULSES_PER_TICK;
        }
        else if (pulse_counter <= -PULSES_PER_TICK)
        {
            tick_direction = direction_e::LEFT;
            pulse_counter += PULSES_PER_TICK;
        }

        encoder_state.last_pulses = encoder_state.pulses;
        encoder_state.last_button_pressed = encoder_state.button_pressed;

        switch (safe_state.state)
        {
        case SAFE_STATE_WAITING:
            // Handle button click and first tick input to start entering the code
            if (button_clicked)
            {
                safe_state.attempts++;

                set_gpio_attempts(safe_state.attempts);

                reset_input(
                    &safe_state,
                    &pulse_counter);

                if (safe_state.attempts >= ATTEMPT_MAX)
                {
                    play_sound_error(&tone);
                    safe_state.state = SAFE_STATE_LOCKED;
                }

                break;
            }

            if (tick_direction != direction_e::NONE)
            {
                safe_state.last_direction = tick_direction;
                safe_state.current_digit = 1;
                ESP_LOGI(
                    TAG,
                    "INPUT: %d",
                    safe_state.current_digit);

                safe_state.state = SAFE_STATE_ENTERING;
            }

            break;

        case SAFE_STATE_ENTERING:
            // Handle button click or tick input to enter the code
            if (button_clicked)
            {
                safe_state.attempts++;

                set_gpio_attempts(safe_state.attempts);
                reset_input(&safe_state, &pulse_counter);

                if (safe_state.attempts >= ATTEMPT_MAX)
                {
                    safe_state.state = SAFE_STATE_LOCKED;
                }
                else
                {
                    safe_state.state = SAFE_STATE_WAITING;
                }

                break;
            }

            if (tick_direction == direction_e::NONE)
            {
                break;
            }

            // If the direction is same as the last tick, increment the current digit
            if (tick_direction == safe_state.last_direction)
            {

                safe_state.current_digit = (safe_state.current_digit + 1) % 10;
                ESP_LOGI(
                    TAG,
                    "INPUT: %d",
                    safe_state.current_digit);

                break;
            }

            // If the direction is different from the last tick, confirm the current digit and move to the next one
            safe_state.input_code[safe_state.code_index] = safe_state.current_digit;
            safe_state.code_index++;
            safe_state.last_direction = tick_direction;

            pulse_counter = 0;

            ESP_LOGI(
                TAG,
                "Digit confirmed: %d",
                safe_state.current_digit);

            if (safe_state.code_index == CODE_LENGTH)
            {
                safe_state.state = SAFE_STATE_VALIDATING;
                break;
            }
            safe_state.current_digit = 0;

            ESP_LOGI(
                TAG,
                "INPUT: %d",
                safe_state.current_digit);

            break;

        case SAFE_STATE_VALIDATING:
            // Validate the entered code
            printf("Entered code: ");

            for (uint8_t i = 0; i < CODE_LENGTH; ++i)
            {
                printf("%d", safe_state.input_code[i]);
            }

            printf("\n");

            if (is_code_valid(safe_state.input_code))
            {

                set_gpio_success(true);

                ESP_LOGI(
                    TAG,
                    "Correct code! SAFE UNLOCKED.");

                play_sound_success(&tone);
                servo_set_angle(&servo, 0);

                reset_input(&safe_state, &pulse_counter);

                safe_state.state = SAFE_STATE_WAITING;
            }
            else
            {
                safe_state.attempts++;

                set_gpio_attempts(safe_state.attempts);

                ESP_LOGW(
                    TAG,
                    "Wrong code. Attempt %d/%d",
                    safe_state.attempts,
                    ATTEMPT_MAX);

                if (safe_state.attempts >= ATTEMPT_MAX)
                {
                    play_sound_error(&tone);
                    safe_state.state = SAFE_STATE_LOCKED;
                }
                else
                {
                    reset_input(&safe_state, &pulse_counter);
                    safe_state.state = SAFE_STATE_WAITING;
                }
            }

            break;

        case SAFE_STATE_LOCKED:
            set_gpio_locked(true);

            ESP_LOGE(
                TAG,
                "SAFE LOCKED. Restart MCU to unlock.");

            vTaskDelay(pdMS_TO_TICKS(10000));

            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}