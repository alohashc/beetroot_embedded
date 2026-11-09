#include "at24c32.h"

#define AT24C32_TIMEOUT 100
#define AT24C32_READY_TRIALS 100

HAL_StatusTypeDef AT24C32_Init(
    AT24C32_t *ctx,
    I2C_HandleTypeDef *hi2c,
    uint8_t address)
{
    if (ctx == NULL || hi2c == NULL)
    {
        return HAL_ERROR;
    }

    if (address < 0x50 || address > 0x57)
    {
        return HAL_ERROR;
    }

    ctx->hi2c = hi2c;
    ctx->device_address = address;

    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(
        ctx->hi2c,
        ctx->device_address << 1,
        AT24C32_READY_TRIALS,
        AT24C32_TIMEOUT);

    return status;
}

HAL_StatusTypeDef AT24C32_Read(
    AT24C32_t *ctx,
    uint16_t address,
    uint8_t *buffer,
    uint16_t size)
{
    if (ctx == NULL || buffer == NULL || size == 0)
    {
        return HAL_ERROR;
    }

    if (address >= AT24C32_SIZE ||
        size > AT24C32_SIZE - address)
    {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        ctx->hi2c,
        ctx->device_address << 1,
        address,
        I2C_MEMADD_SIZE_16BIT,
        buffer,
        size,
        AT24C32_TIMEOUT);

    return status;
}

HAL_StatusTypeDef AT24C32_Write(
    AT24C32_t *ctx,
    uint16_t address,
    const uint8_t *buffer,
    uint16_t size)
{
    if (ctx == NULL || ctx->hi2c == NULL || buffer == NULL)
    {
        return HAL_ERROR;
    }

    if (address >= AT24C32_SIZE ||
        size > AT24C32_SIZE - address)
    {
        return HAL_ERROR;
    }

    if (size == 0)
    {
        return HAL_OK;
    }

    while (size > 0)
    {
        uint16_t page_offset = address % AT24C32_PAGE_SIZE;
        uint16_t page_space = AT24C32_PAGE_SIZE - page_offset;
        uint16_t write_size = (size < page_space) ? size : page_space;

        HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
            ctx->hi2c,
            ctx->device_address << 1,
            address,
            I2C_MEMADD_SIZE_16BIT,
            (uint8_t *)buffer,
            write_size,
            AT24C32_TIMEOUT);

        if (status != HAL_OK)
        {
            return status;
        }

        status = HAL_I2C_IsDeviceReady(
            ctx->hi2c,
            ctx->device_address << 1,
            AT24C32_READY_TRIALS,
            AT24C32_TIMEOUT);

        if (status != HAL_OK)
        {
            return status;
        }

        address += write_size;
        buffer += write_size;
        size -= write_size;
    }

    return HAL_OK;
}
