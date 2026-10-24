#ifndef DS1307_H
#define DS1307_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "stm32f4xx_hal.h"

struct DS1307_Time {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t dayOfWeek;
    uint8_t day;
    uint8_t month;
    uint16_t year;
};

HAL_StatusTypeDef DS1307_ReadTime(
    I2C_HandleTypeDef* hi2c,
    DS1307_Time* time
);

HAL_StatusTypeDef DS1307_SetTime(
    I2C_HandleTypeDef *hi2c,
    const DS1307_Time *time
);

#ifdef __cplusplus
}
#endif

#endif