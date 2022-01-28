#include <dev/bus.h>

#define ALIGN_DOWN(NUM, WHAT) ((NUM) & ~(WHAT - 1))

void bus_init(Bus *bus)
{
    bus->device_count = 0;
}

void bus_attach(BusDevice dev, Bus *bus)
{
    bus->devices[bus->device_count++] = dev;
}

void bus_read(uint32_t addr, MemSize size, uint32_t *value, Ram *ram)
{
    if (addr >= MEMORY_SIZE + 0x1000 && addr < MEMORY_SIZE + 0x2000)
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

        info("reading device at index %d", index);
    }

    else
    {
        ram_read(addr, size, value, ram);
    }

    return;
}