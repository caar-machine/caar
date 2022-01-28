#include <dev/bus.h>

void bus_read(uint32_t addr, MemSize size, uint32_t *value, Ram *ram)
{
    if (addr >= 0xc0000000 && addr < 0xc0001000)
    {
        return;
    }

    else
    {
        ram_read(addr, size, value, ram);
    }

    return;
}