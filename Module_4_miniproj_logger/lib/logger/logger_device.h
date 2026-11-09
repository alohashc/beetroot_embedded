#ifndef LOGGER_DEVICE_H
#define LOGGER_DEVICE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef bool (*LoggerDevice_ReadFn)(
        void *context,
        uint32_t address,
        uint8_t *buffer,
        size_t size);

    typedef bool (*LoggerDevice_WriteFn)(
        void *context,
        uint32_t address,
        const uint8_t *buffer,
        size_t size);

    typedef struct
    {
        void *context;

        LoggerDevice_ReadFn read;
        LoggerDevice_WriteFn write;

        uint32_t capacity;
    } LoggerDevice_t;

    bool LoggerDevice_Init(
        LoggerDevice_t *device,
        void *context,
        LoggerDevice_ReadFn read,
        LoggerDevice_WriteFn write,
        uint32_t capacity);

#ifdef __cplusplus
}
#endif

#endif
