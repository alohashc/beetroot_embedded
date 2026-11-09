#include "logger.h"

#include <stdio.h>

static bool Logger_IsPageEmpty(const uint8_t *buffer)
{
    return buffer[0] == 0xFF || buffer[0] == 0x00;
}

static uint16_t Logger_ParseNumber(const uint8_t *buffer)
{
    uint16_t number = 0;
    uint8_t index = 1; // Skip '#'

    while (index < 4 &&
           buffer[index] >= '0' &&
           buffer[index] <= '9')
    {

        number = number * 10 + (buffer[index] - '0');
        index++;
    }

    return number;
}

bool Logger_Init(Logger_t *logger, LoggerDevice_t *device)
{
    if (logger == NULL ||
        device == NULL ||
        device->context == NULL ||
        device->read == NULL ||
        device->write == NULL ||
        device->capacity < LOGGER_RECORD_SIZE)
    {
        return false;
    }

    logger->device = device;
    logger->recordCount = 0;
    logger->writePage = 0;
    logger->nextLogNumber = 0;
    logger->pageCount =
        device->capacity / LOGGER_RECORD_SIZE;

    uint8_t buffer[LOGGER_RECORD_SIZE];

    uint16_t previousNumber = 0;
    uint16_t lastNumber = 0;
    bool hasLogs = false;
    bool foundBoundary = false;

    for (uint16_t page = 0; page < logger->pageCount; page++)
    {

        uint32_t address = (uint32_t)page * LOGGER_RECORD_SIZE;

        if (!device->read(
                device->context,
                address,
                buffer,
                LOGGER_RECORD_SIZE))
        {
            return false;
        }

        if (Logger_IsPageEmpty(buffer))
        {
            logger->writePage = page;
            logger->nextLogNumber =
                hasLogs ? (lastNumber + 1) % 1000 : 0;

            return true;
        }

        uint16_t number = Logger_ParseNumber(buffer);

        if (hasLogs &&
            number != (previousNumber + 1) % 1000)
        {

            logger->writePage = page;
            logger->nextLogNumber =
                (previousNumber + 1) % 1000;

            foundBoundary = true;
        }

        previousNumber = number;
        lastNumber = number;
        hasLogs = true;
        logger->recordCount++;
    }

    if (!foundBoundary)
    {
        logger->writePage = 0;
        logger->nextLogNumber = (lastNumber + 1) % 1000;
    }

    return true;
}

bool Logger_Write(Logger_t *logger, const char *message)
{
    if (logger == NULL ||
        logger->device == NULL ||
        logger->device->write == NULL ||
        message == NULL)
    {
        return false;
    }

    char buffer[LOGGER_RECORD_SIZE] = {0};

    snprintf(
        buffer,
        sizeof(buffer),
        "#%u %s",
        (unsigned int)logger->nextLogNumber,
        message
    );

    uint32_t address =
        (uint32_t)logger->writePage * LOGGER_RECORD_SIZE;

    if (!logger->device->write(
            logger->device->context,
            address,
            (const uint8_t *)buffer,
            LOGGER_RECORD_SIZE))
    {
        return false;
    }

    if (logger->recordCount < logger->pageCount)
    {
        logger->recordCount++;
    }

    logger->writePage =
        (logger->writePage + 1) % logger->pageCount;

    logger->nextLogNumber =
        (logger->nextLogNumber + 1) % 1000;

    return true;
}

bool Logger_Read(
    Logger_t *logger,
    uint16_t logNumber,
    char *buffer,
    size_t bufferSize)
{
    if (logger == NULL ||
        logger->device == NULL ||
        logger->device->read == NULL ||
        buffer == NULL ||
        bufferSize < LOGGER_RECORD_SIZE ||
        logNumber > LOGGER_NUMBER_MAX ||
        logger->pageCount == 0)
    {
        return false;
    }

    uint16_t distance =
        (logger->nextLogNumber + 1000 - logNumber) % 1000;

    if (distance == 0 || distance > logger->recordCount)
    {
        return false;
    }

    uint16_t page =
        (logger->writePage + logger->pageCount - distance)
        % logger->pageCount;

    uint32_t address =
        (uint32_t)page * LOGGER_RECORD_SIZE;

    return logger->device->read(
        logger->device->context,
        address,
        (uint8_t *)buffer,
        LOGGER_RECORD_SIZE
    );
}

uint16_t Logger_GetCount(const Logger_t *logger)
{
    if (logger == NULL)
    {
        return 0;
    }

    return logger->recordCount;
}

bool Logger_GetFirstNumber(const Logger_t *logger, uint16_t *number)
{
    if (logger == NULL ||
        number == NULL ||
        logger->recordCount == 0)
    {
        return false;
    }

    *number =
        (logger->nextLogNumber +
         1000 -
         logger->recordCount) % 1000;

    return true;
}

bool Logger_GetLastNumber(const Logger_t *logger, uint16_t *number)
{
    if (logger == NULL ||
        number == NULL ||
        logger->recordCount == 0)
    {
        return false;
    }

    *number =
        (logger->nextLogNumber + 999) % 1000;

    return true;
}

bool Logger_Clear(Logger_t *logger)
{
    if (logger == NULL ||
        logger->device == NULL ||
        logger->device->write == NULL ||
        logger->pageCount == 0)
    {
        return false;
    }

    uint8_t buffer[LOGGER_RECORD_SIZE] = {0};

    for (uint16_t page = 0; page < logger->pageCount; page++)
    {
        uint32_t address =
            (uint32_t)page * LOGGER_RECORD_SIZE;

        if (!logger->device->write(
                logger->device->context,
                address,
                buffer,
                LOGGER_RECORD_SIZE))
        {
            return false;
        }
    }

    logger->recordCount = 0;
    logger->writePage = 0;
    logger->nextLogNumber = 0;

    return true;
}
