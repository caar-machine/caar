#include "ram.h"
#include <dev/bus.h>
#include <dev/gpu.h>
#include <stdlib.h>

#define ALIGN_DOWN(NUM, WHAT) ((NUM) & ~(WHAT - 1))

void bus_init(Bus *bus)
{
    bus->device_count = 0;
}

void bus_attach(BusDevice dev, Bus *bus)
{
    bus->devices[bus->device_count++] = dev;
}

RawBus *raw_bus = NULL;

RawBus *bus_to_raw_bus(Bus bus)
{
    if (!raw_bus)
    {
        raw_bus = malloc(sizeof(RawBus));

        raw_bus->device_count = bus.device_count;

        for (size_t i = 0; i < bus.device_count; i++)
        {
            raw_bus->devices[i].type = bus.devices[i].type;

            raw_bus->devices[i].addr = i + MEMORY_SIZE + 0x1000;
        }
    }

    return raw_bus;
}

void bus_write(uint32_t addr, MemSize size, uint32_t value, Ram *ram, Bus *bus)
{

    if (addr >= MEMORY_SIZE + 1 && addr < MEMORY_SIZE + 0x1000)
    {
        warn("attempt to write at bus");
    }

    else if (addr >= FB_ADDRESS && addr <= FB_ADDRESS + FB_SIZE)
    {
        gpu_draw(value, addr);
    }

    else if (addr >= MEMORY_SIZE + 0x1000 && addr < MEMORY_SIZE + 0x2000)
    {
        size_t index = addr - MEMORY_SIZE - 0x1000;

        if (index > bus->device_count)
            error("Out of bounds write to bus.");

        bus->devices[index].write(value, ram);
    }

    else
    {
        ram_write(addr, size, value, ram);
    }

    return;
}

void bus_read(uint32_t addr, MemSize size, uint32_t *value, Ram *ram, Bus *bus)
{

    if (addr >= MEMORY_SIZE + 1 && addr < MEMORY_SIZE + 0x1000)
    {
        RawBus *raw = bus_to_raw_bus(*bus);

        *value = *(uint32_t *)((uint64_t)raw + (addr - (MEMORY_SIZE + 1)));
    }

    else if (addr >= MEMORY_SIZE + 0x1000 && addr < MEMORY_SIZE + 0x2000)
    {

        size_t index = addr - MEMORY_SIZE - 0x1000;

        if (index <= 128)
        {
            index = 0;
        }

        else
        {
            index = ALIGN_DOWN(index, 128) / 128;
        }

        if (index > bus->device_count)
            error("Out of bounds read to bus.");

        *value = bus->devices[index].read();
    }

    else
    {
        ram_read(addr, size, (uint32_t *)value, ram);
    }

    return;
}

uint32_t bus_get_addr()
{
    return MEMORY_SIZE + 1;
}
