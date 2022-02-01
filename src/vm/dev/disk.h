#ifndef CAAR_DISK_H
#define CAAR_DISK_H
#include <dev/bus.h>

typedef struct __attribute__((packed))
{
    uint8_t type;
    uint8_t sectors;
    uint32_t address;
} DiskCommand;

void disk_init(Bus *bus);

#endif