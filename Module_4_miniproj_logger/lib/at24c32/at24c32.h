#ifndef AT24C32_H
#define AT24C32_H

#include <stdint.h>
#include "main.h"

#define AT24C32_SIZE 4096
#define AT24C32_PAGE_SIZE 32
#define AT24C32_I2C_ADDR 0x50

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint8_t device_address;
} AT24C32_t;

HAL_StatusTypeDef AT24C32_Init(
    AT24C32_t *ctx,
    I2C_HandleTypeDef *hi2c,
    uint8_t address);

HAL_StatusTypeDef AT24C32_Read(
    AT24C32_t *ctx,
    uint16_t address,
    uint8_t *buffer,
    uint16_t size);

HAL_StatusTypeDef AT24C32_Write(
    AT24C32_t *ctx,
    uint16_t address,
    const uint8_t *buffer,
    uint16_t size);

#endif
