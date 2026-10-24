#include "app/ds1307.h"


#define DS1307_I2C_ADDR (0x68 << 1)

static uint8_t BCD_To_Dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t Dec_To_BCD(uint8_t value)
{
    return ((value / 10U) << 4) | (value % 10U);
}

HAL_StatusTypeDef DS1307_ReadTime(
    I2C_HandleTypeDef *hi2c,
    DS1307_Time *time)
{
    if (hi2c == nullptr || time == nullptr) {
        return HAL_ERROR;
    }

    uint8_t buffer[7] = {};

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        hi2c,
        DS1307_I2C_ADDR,
        0x00,
        I2C_MEMADD_SIZE_8BIT,
        buffer,
        sizeof(buffer),
        100
    );

    if (status != HAL_OK) {
        return status;
    }

    // if CH = 1 - generator stopped
    if (buffer[0] & 0x80) {
        return HAL_ERROR;
    }

    // 24 hour format only
    if (buffer[2] & 0x40) {
        return HAL_ERROR;
    }

    DS1307_Time result = {};

    result.seconds = BCD_To_Dec(buffer[0] & 0x7F);
    result.minutes = BCD_To_Dec(buffer[1] & 0x7F);
    result.hours = BCD_To_Dec(buffer[2] & 0x3F);

    result.dayOfWeek = buffer[3] & 0x07;
    result.day = BCD_To_Dec(buffer[4] & 0x3F);
    result.month = BCD_To_Dec(buffer[5] & 0x1F);
    result.year = 2000 + BCD_To_Dec(buffer[6]);

    *time = result;

    return HAL_OK;
}

HAL_StatusTypeDef DS1307_SetTime(
    I2C_HandleTypeDef *hi2c,
    const DS1307_Time *time)
{
    if (hi2c == nullptr || time == nullptr) {
        return HAL_ERROR;
    }

    if (time->seconds > 59 ||
        time->minutes > 59 ||
        time->hours > 23 ||
        time->dayOfWeek < 1 || time->dayOfWeek > 7 ||
        time->day < 1 || time->day > 31 ||
        time->month < 1 || time->month > 12 ||
        time->year < 2000 || time->year > 2099)
    {
        return HAL_ERROR;
    }

    uint8_t data[7] = {
        static_cast<uint8_t>(Dec_To_BCD(time->seconds) & 0x7F),
        Dec_To_BCD(time->minutes),
        Dec_To_BCD(time->hours),
        Dec_To_BCD(time->dayOfWeek),
        Dec_To_BCD(time->day),
        Dec_To_BCD(time->month),
        Dec_To_BCD(static_cast<uint8_t>(time->year - 2000))
    };

    return HAL_I2C_Mem_Write(
        hi2c,
        DS1307_I2C_ADDR,
        0x00,
        I2C_MEMADD_SIZE_8BIT,
        data,
        sizeof(data),
        100
    );
}