#ifndef CAAR_BUS_H
#define CAAR_BUS_H
#include <lib/log.h>
#include <ram.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    void (*write)(void *data);
    uint32_t (*read)();
    int type;
} BusDevice;

typedef struct
{
    BusDevice devices[32];
    size_t device_count;
} Bus;

void bus_init(Bus *bus);
void bus_attach(BusDevice dev, Bus *bus);

void bus_read(uint32_t addr, MemSize size, uint32_t *value, Ram *ram);

#endif