#include "main.h"
#include "at24c32.h"
#include <cstdio>
#include "logger.h"

extern I2C_HandleTypeDef hi2c1;

static bool CallbackWrite(
    void *context,
    uint32_t address,
    const uint8_t *buffer,
    size_t size)
{
    AT24C32_t *device = (AT24C32_t *)context;

    if (device == NULL ||
        buffer == NULL ||
        address >= AT24C32_SIZE ||
        size > AT24C32_SIZE - address)
    {
        return false;
    }

    return AT24C32_Write(
               device,
               (uint16_t)address,
               buffer,
               (uint16_t)size) == HAL_OK;
}

static bool CallbackRead(
    void *context,
    uint32_t address,
    uint8_t *buffer,
    size_t size)
{
    AT24C32_t *device = (AT24C32_t *)context;

    if (device == NULL ||
        buffer == NULL ||
        address >= AT24C32_SIZE ||
        size > AT24C32_SIZE - address)
    {
        return false;
    }

    return AT24C32_Read(
               device,
               (uint16_t)address,
               buffer,
               (uint16_t)size) == HAL_OK;
}

extern "C" void app_main()
{
    HAL_Delay(10000); // Wait for USB to be ready

    // Initialize the I2C peripheral
    AT24C32_t device;
    AT24C32_Init(
        &device,
        &hi2c1,
        AT24C32_I2C_ADDR);

    // Initialize the logger device and logger
    LoggerDevice_t logger_device;
    Logger_t logger;

    LoggerDevice_Init(
        &logger_device,
        &device,
        CallbackRead,
        CallbackWrite,
        AT24C32_SIZE);

    Logger_Init(&logger, &logger_device);

    // Clear the logger and write some log messages
    Logger_Clear(&logger);

    // Write 10 log messages
    for (uint16_t i = 0; i < 10; i++)
    {
        char message[LOGGER_RECORD_SIZE];
        snprintf(message, sizeof(message), "Log message %u", (unsigned int)i);

        Logger_Write(&logger, message);
    }

    // Check the count of log messages
    printf("Count: %u\r\n", Logger_GetCount(&logger));

    // Get the first log number
    uint16_t first = 0;

    if (Logger_GetFirstNumber(&logger, &first))
    {
        printf("First: #%u\r\n", first);
    }

    // Get the last log number
    uint16_t last = 0;

    if (Logger_GetLastNumber(&logger, &last))
    {
        printf("Last: #%u\r\n", last);
    }

    // Read the log message with number 2
    char buffer[LOGGER_RECORD_SIZE + 1] = {0};

    if (Logger_Read(&logger, 2, buffer, sizeof(buffer)))
    {
        printf("Read #2: %s\r\n", buffer);
    }

    // Demonstrate restoring the logger state after a reset
    Logger_t restored = {0};

    printf(
        "Restore: %d\r\n",
        Logger_Init(&restored, &logger_device));

    printf("Restored count: %u\r\n", Logger_GetCount(&restored));

    // Write more than 999 log messages
    for (uint16_t i = Logger_GetCount(&restored); i < 1010; i++)
    {
        char message[LOGGER_RECORD_SIZE];
        snprintf(message, sizeof(message), "Log message %u", (unsigned int)i);

        Logger_Write(&restored, message);
    }

    // Check the count of log messages
    printf("Count: %u\r\n", Logger_GetCount(&restored));

    // Get the first log number
    if (Logger_GetFirstNumber(&restored, &first))
    {
        printf("First: #%u\r\n", first);
    }

    // Get the last log number
    if (Logger_GetLastNumber(&restored, &last))
    {
        printf("Last: #%u\r\n", last);
    }

    // Read the log message with number 999
    if (Logger_Read(&restored, 999, buffer, sizeof(buffer)))
    {
        printf("Read # 999: %s\r\n", buffer);
    }

    while (1)
    {
    }
}
