#ifndef CAAR_RAM_H
#define CAAR_RAM_H
#include <stdint.h>

// 4 mb
#define MEMORY_SIZE 1024 * 1024 * 16

typedef struct
{
    uint8_t *buffer;
    uint32_t allocator_index;
} Ram;

void ram_init(Ram *ram);
void ram_write(uint32_t addr, uint32_t data, Ram *ram);
uint8_t ram_read(uint32_t addr, Ram *ram);

void *ram_allocate(Ram *ram);
void ram_free(void *ptr, Ram *ram);

#endif