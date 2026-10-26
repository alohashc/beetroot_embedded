#include "app/bmp280.h"

constexpr uint8_t BMP280_SPI_WRITE_MASK = 0x7F;

void BMP280_Select()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}

void BMP280_Deselect()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

HAL_StatusTypeDef BMP280_WriteRegister(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t value
) {
    uint8_t tx[2] = {
        static_cast<uint8_t>(reg & BMP280_SPI_WRITE_MASK),
        value
    };

    BMP280_Select();

    HAL_StatusTypeDef status = HAL_SPI_Transmit(
        hspi,
        tx,
        sizeof(tx),
        100
    );

    BMP280_Deselect();

    return status;
}

HAL_StatusTypeDef BMP280_ReadRegisters(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t *buffer,
    uint16_t size
)
{
    if (buffer == nullptr || size == 0) {
        return HAL_ERROR;
    }

    uint8_t address = reg | 0x80;

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

    HAL_StatusTypeDef status = HAL_SPI_Transmit(
        hspi, &address, 1, 100
    );

    if (status == HAL_OK) {
        status = HAL_SPI_Receive(
            hspi, buffer, size, 100
        );
    }

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

    return status;
}
