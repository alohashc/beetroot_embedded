#ifndef BMP280_H
#define BMP280_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "stm32f4xx_hal.h"

void BMP280_Select(void);
void BMP280_Deselect(void);

HAL_StatusTypeDef BMP280_WriteRegister(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t value
);

HAL_StatusTypeDef BMP280_ReadRegisters(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t *buffer,
    uint16_t size
);

#ifdef __cplusplus
}
#endif

#endif