
#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>
#include <stdint.h>

#include "logger_device.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define LOGGER_RECORD_SIZE 32
#define LOGGER_NUMBER_MAX 999

    typedef struct
    {
        LoggerDevice_t *device;

        uint16_t recordCount;
        uint16_t writePage;
        uint16_t nextLogNumber;
        uint16_t pageCount;
    } Logger_t;

    bool Logger_Init(Logger_t *logger, LoggerDevice_t *device);

    bool Logger_Write(Logger_t *logger, const char *message);
    bool Logger_Read(Logger_t *logger, uint16_t logNumber, char *buffer, size_t bufferSize);
    uint16_t Logger_GetCount(const Logger_t *logger);
    bool Logger_GetFirstNumber(const Logger_t *logger, uint16_t *number);
    bool Logger_GetLastNumber(const Logger_t *logger, uint16_t *number);
    bool Logger_Clear(Logger_t *logger);

#ifdef __cplusplus
}
#endif

#endif
