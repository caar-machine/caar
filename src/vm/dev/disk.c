#include "dev/bus.h"
#include <dev/disk.h>

uint32_t disk_read()
{
    return 0;
}

void disk_write(void *data, Ram *ram)
{

    (void)ram;

    DiskCommand *command = (DiskCommand *)data;

    if (command->type == 0) // Read
    {
        warn("TODO: read");
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