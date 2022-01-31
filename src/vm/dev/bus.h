#ifndef CAAR_BUS_H
#define CAAR_BUS_H
#include <lib/log.h>
#include <ram.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    BUS_DEV_EMPTY,
    BUS_DEV_DISK,
    BUS_DEV_GPU,
} DeviceType;

typedef struct __attribute__((packed))
{
    void (*write)(void *data, Ram *ram);
    uint32_t (*read)();
    DeviceType type;

} BusDevice;

typedef struct
{
    BusDevice devices[32];
    size_t device_count;
} Bus;

typedef struct __attribute__((packed))
{
    uint32_t type;
    uint32_t addr;
} RawBusDevice;

typedef struct __attribute__((packed))
{
    uint32_t device_count;
    RawBusDevice devices[32];
} RawBus;

void bus_init(Bus *bus);
void bus_attach(BusDevice dev, Bus *bus);

void bus_read(uint32_t addr, MemSize size, uint32_t *value, Ram *ram, Bus *bus);

RawBus *bus_to_raw_bus(Bus bus);

uint32_t bus_get_addr();

#endif