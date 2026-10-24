#include <cstdio>
#include "main.h"
#include "app/ds1307.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

extern "C" I2C_HandleTypeDef hi2c1;

static void display_time(const DS1307_Time *time)
{
    if (time == nullptr) {
        return;
    }

    static const char *weekdays[] = {
        "Sun", "Mon", "Tue", "Wed",
        "Thu", "Fri", "Sat"
    };

    if (time->dayOfWeek < 1 || time->dayOfWeek > 7) {
        return;
    }

    char timeStr[16];
    char dateStr[24];

    snprintf(
        timeStr,
        sizeof(timeStr),
        "%02u:%02u:%02u",
        (unsigned)time->hours,
        (unsigned)time->minutes,
        (unsigned)time->seconds
    );

    snprintf(
        dateStr,
        sizeof(dateStr),
        "%s %02u.%02u.%04u",
        weekdays[time->dayOfWeek - 1],
        (unsigned)time->day,
        (unsigned)time->month,
        (unsigned)time->year
    );

    ssd1306_Fill(Black);

    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(timeStr, Font_11x18, White);

    ssd1306_SetCursor(0, 24);
    ssd1306_WriteString(dateStr, Font_7x10, White);

    ssd1306_UpdateScreen();
}

extern "C" void main_cpp(void)
{
    ssd1306_Init();

    uint32_t lastUpdate = HAL_GetTick() - 1000U;

    while (1)
    {
        uint32_t now = HAL_GetTick();

        if ((uint32_t)(now - lastUpdate) >= 1000U)
        {
            lastUpdate += 1000U;

            DS1307_Time time = {};

            if (DS1307_ReadTime(&hi2c1, &time) == HAL_OK)
            {
                display_time(&time);
            }
            else
            {
                printf("DS1307 read error!\n");
            }
        }
    }
}