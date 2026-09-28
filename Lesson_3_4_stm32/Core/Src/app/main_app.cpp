#include <stdio.h>
#include <stdbool.h>
#include "main.h"

#define TIMER_CHANNEL TIM_CHANNEL_1
#define PRESCALER 97

#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523

constexpr uint8_t SINE_SAMPLES = 16;
constexpr uint8_t sine_table[SINE_SAMPLES] = {
    50, 69, 85, 96,
    100, 96, 85, 69,
    50, 31, 15, 4,
    0, 4, 15, 31
};

extern TIM_HandleTypeDef htim3;

static uint32_t sample_index = 0;

void Buzzer_Stop()
{
    __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
    __HAL_TIM_SET_COMPARE(&htim3, TIMER_CHANNEL, 0);
}

uint32_t Timer_GetCounterFrequency(TIM_HandleTypeDef *htim)
{
    RCC_ClkInitTypeDef clock_config = {};
    uint32_t flash_latency = 0;

    uint32_t timer_clock = HAL_RCC_GetPCLK1Freq();

    HAL_RCC_GetClockConfig(&clock_config, &flash_latency);

    if (clock_config.APB1CLKDivider != RCC_HCLK_DIV1)
    {
        timer_clock *= 2;
    }

    return timer_clock / (PRESCALER + 1);
}

void Buzzer_SetFrequency(uint32_t frequency_hz)
{
    if (frequency_hz == 0) {
        Buzzer_Stop();
        return;
    }

    uint32_t counter_hz = Timer_GetCounterFrequency(&htim3);
    uint32_t update_hz = frequency_hz * SINE_SAMPLES;
    uint32_t period_ticks = counter_hz / update_hz;
    uint32_t arr = period_ticks - 1;

    __HAL_TIM_SET_AUTORELOAD(&htim3, arr);
    __HAL_TIM_SET_COUNTER(&htim3, 0);

    sample_index = 0;

    __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
}

static void Sound_UpdateSample()
{
    uint32_t duty = sine_table[sample_index];
    uint32_t period_ticks = __HAL_TIM_GET_AUTORELOAD(&htim3) + 1;
    uint32_t ccr = (period_ticks * duty) / 100;

    __HAL_TIM_SET_COMPARE(&htim3, TIMER_CHANNEL, ccr);

    sample_index++;

    if (sample_index >= SINE_SAMPLES) {
        sample_index = 0;
    }
}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        Sound_UpdateSample();
    }
}

void Sound_Pause(uint32_t duration_ms)
{
    Buzzer_Stop();
    HAL_Delay(duration_ms);
}

void Sound_PlayNote(uint32_t frequency_hz, uint32_t duration_ms)
{
    Buzzer_SetFrequency(frequency_hz);

    HAL_Delay(duration_ms);

    Buzzer_Stop();
}

extern "C" void main_cpp()
{
    HAL_TIM_PWM_Start(&htim3, TIMER_CHANNEL);
    __HAL_TIM_SET_PRESCALER(&htim3, PRESCALER);

    while (1)
    {
        // E E E
        Sound_PlayNote(NOTE_E4, 250);
        Sound_Pause(80);

        Sound_PlayNote(NOTE_E4, 250);
        Sound_Pause(80);

        Sound_PlayNote(NOTE_E4, 500);

        Sound_Pause(300);


        // E E E
        Sound_PlayNote(NOTE_E4, 250);
        Sound_Pause(80);

        Sound_PlayNote(NOTE_E4, 250);
        Sound_Pause(80);

        Sound_PlayNote(NOTE_E4, 500);

        Sound_Pause(300);


        // E G C D E
        Sound_PlayNote(NOTE_E4, 250);
        Sound_Pause(80);

        Sound_PlayNote(NOTE_G4, 250);
        Sound_Pause(80);

        Sound_PlayNote(NOTE_C4, 250);
        Sound_Pause(80);

        Sound_PlayNote(NOTE_D4, 250);
        Sound_Pause(80);

        Sound_PlayNote(NOTE_E4, 500);

        Sound_Pause(1000);
    }
}