#include <cstdio>
#include "main.h"
#include "app/ds1307.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "app/bmp280.h"

extern "C" {
#include "bmp280.h"
}

extern "C" I2C_HandleTypeDef hi2c1;
extern "C" SPI_HandleTypeDef hspi1;

extern "C" int8_t BMP280_SPI_Read_Callback(
    uint8_t dev_addr,
    uint8_t reg_addr,
    uint8_t *data,
    uint8_t len)
{
    (void)dev_addr;

    HAL_StatusTypeDef status = BMP280_ReadRegisters(
        &hspi1,
        reg_addr,
        data,
        len);

    return (status == HAL_OK) ? 0 : -1;
}

extern "C" int8_t BMP280_SPI_Write_Callback(
    uint8_t dev_addr,
    uint8_t reg_addr,
    uint8_t *data,
    uint8_t len)
{
    (void)dev_addr;

    if (data == nullptr || len == 0)
    {
        return -1;
    }

    for (uint8_t i = 0; i < len; ++i)
    {
        if (BMP280_WriteRegister(
                &hspi1,
                reg_addr + i,
                data[i]) != HAL_OK)
        {
            return -1;
        }
    }

    return 0;
}

static void display_data(
    const DS1307_Time *time,
    float temperature_c,
    float pressure_hpa)
{
    if (time == nullptr)
    {
        return;
    }

    static const char *weekdays[] = {
        "Sun", "Mon", "Tue", "Wed",
        "Thu", "Fri", "Sat"};

    if (time->dayOfWeek < 1 || time->dayOfWeek > 7)
    {
        return;
    }

    char timeStr[16];
    char dateStr[24];
    char temperatureStr[24];
    char pressureStr[24];

    snprintf(
        timeStr, sizeof(timeStr),
        "%02u:%02u:%02u",
        (unsigned)time->hours,
        (unsigned)time->minutes,
        (unsigned)time->seconds);

    snprintf(
        dateStr, sizeof(dateStr),
        "%s %02u.%02u.%04u",
        weekdays[time->dayOfWeek - 1],
        (unsigned)time->day,
        (unsigned)time->month,
        (unsigned)time->year);

    snprintf(
        temperatureStr, sizeof(temperatureStr),
        "T: %.2f C", (double)temperature_c);

    snprintf(
        pressureStr, sizeof(pressureStr),
        "P: %.1f hPa", (double)pressure_hpa);

    ssd1306_Fill(Black);

    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(timeStr, Font_11x18, White);

    ssd1306_SetCursor(0, 22);
    ssd1306_WriteString(dateStr, Font_7x10, White);

    ssd1306_SetCursor(0, 38);
    ssd1306_WriteString(temperatureStr, Font_7x10, White);

    ssd1306_SetCursor(0, 50);
    ssd1306_WriteString(pressureStr, Font_7x10, White);

    ssd1306_UpdateScreen();
}

extern "C" void main_cpp(void)
{
    // SET TIME FOR DS1307
    // DS1307_Time time = {};

    // time.hours = 18;
    // time.minutes = 47;
    // time.seconds = 0;
    // time.day = 24;
    // time.month = 10;
    // time.year = 2026;
    // time.dayOfWeek = 7;

    // HAL_StatusTypeDef status = DS1307_SetTime(&hi2c1, &time);
    
    ssd1306_Init();

    static struct bmp280_t bmp = {};

    bmp.bus_read = BMP280_SPI_Read_Callback;
    bmp.bus_write = BMP280_SPI_Write_Callback;

    if (bmp280_init(&bmp) != 0)
    {
        Error_Handler();
    }

    if (bmp280_set_oversamp_temperature(BMP280_OVERSAMP_1X) != 0 ||
        bmp280_set_oversamp_pressure(BMP280_OVERSAMP_1X) != 0 ||
        bmp280_set_power_mode(BMP280_NORMAL_MODE) != 0)
    {
        Error_Handler();
    }

    uint32_t lastUpdate = HAL_GetTick() - 1000U;

    while (1)
    {
        uint32_t now = HAL_GetTick();

        if ((uint32_t)(now - lastUpdate) >= 1000U)
        {
            lastUpdate += 1000U;

            DS1307_Time time = {};

            HAL_StatusTypeDef rtcStatus =
                DS1307_ReadTime(&hi2c1, &time);

            s32 adc_P = 0;
            s32 adc_T = 0;

            int8_t bmpStatus =
                bmp280_read_uncomp_pressure_temperature(
                    &adc_P,
                    &adc_T);

            if (rtcStatus == HAL_OK && bmpStatus == 0)
            {
                int32_t temperature_raw =
                    bmp280_compensate_T_int32(adc_T);

                uint32_t pressure_pa =
                    bmp280_compensate_P_int32(adc_P);

                float temperature_c = temperature_raw / 100.0f;
                float pressure_hpa = pressure_pa / 100.0f;

                display_data(&time, temperature_c, pressure_hpa);
            }
            else
            {
                printf("Sensor read error!\n");
            }
        }
    }
}