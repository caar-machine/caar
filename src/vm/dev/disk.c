#include <dev/disk.h>
#include <stdlib.h>
#include <string.h>

uint32_t disk_read()
{
    return 0;
}

uint8_t *get_disk_data();

void disk_write(uint32_t address, Ram *ram)
{

    uint8_t bytes[6] = {0};

    ram_read(address, MEM_BYTE, (uint32_t *)&bytes[0], ram);
    ram_read(address + 1, MEM_BYTE, (uint32_t *)&bytes[1], ram);
    ram_read(address + 2, MEM_4_BYTES, (uint32_t *)(bytes + 2), ram);

    DiskCommand *command = (DiskCommand *)bytes;

    if (command->type == 0) // Read
    {
        for (int i = 0; i < 512 * command->sectors; i++)
        {
            ram_write(command->address + i, MEM_BYTE, get_disk_data()[i], ram);
        }
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