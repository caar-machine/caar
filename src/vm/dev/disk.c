#include "dev/bus.h"
#include "ram.h"
#include <dev/disk.h>

uint32_t disk_read()
{
    return 0;
}

void disk_write(uint32_t address, Ram *ram)
{

    uint8_t bytes[6] = {0};

    ram_read(address, MEM_BYTE, (uint32_t *)&bytes[0], ram);
    ram_read(address + 1, MEM_BYTE, (uint32_t *)&bytes[1], ram);
    ram_read(address + 2, MEM_4_BYTES, (uint32_t *)(bytes + 2), ram);

    DiskCommand *command = (DiskCommand *)bytes;

    if (command->type == 0) // Read
    {
        info("reading %d sector(s) at %x", command->sectors, command->address);
    }

    else if (command->type == 1) // Write
    {
        warn("TODO: write");
    }
}

void disk_init(Bus *bus)
{
    BusDevice dev = {.read = disk_read, .write = disk_write, .type = BUS_DEV_DISK};

    bus_attach(dev, bus);
}