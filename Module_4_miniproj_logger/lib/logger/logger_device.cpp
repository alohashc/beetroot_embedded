#include "logger_device.h"

bool LoggerDevice_Init(
    LoggerDevice_t *device,
    void *context,
    LoggerDevice_ReadFn read,
    LoggerDevice_WriteFn write,
    uint32_t capacity)
{
    if (device == NULL ||
        context == NULL ||
        read == NULL ||
        write == NULL ||
        capacity == 0)
    {
        return false;
    }

    device->context = context;
    device->read = read;
    device->write = write;
    device->capacity = capacity;

    return true;
}
