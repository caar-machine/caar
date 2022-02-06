#ifndef CAAR_RAM_H
#define CAAR_RAM_H
#include <stdint.h>

uint32_t get_memory_size();

#define MEMORY_SIZE get_memory_size()

static inline uint32_t to_le(uint32_t num)
{
    return ((num >> 24) & 0xff) |
           ((num << 8) & 0xff0000) |
           ((num >> 8) & 0xff00) |
           ((num << 24) & 0xff000000);
}

typedef enum
{
    MEM_BYTE,
    MEM_2_BYTES,
    MEM_4_BYTES,
} MemSize;
typedef struct
{
    uint8_t *buffer;
    uint32_t allocator_index;
} Ram;

void ram_init(Ram *ram);
void ram_write(uint32_t addr, MemSize size, uint32_t data, Ram *ram);
void ram_read(uint32_t addr, MemSize size, uint32_t *value, Ram *ram);

void *ram_allocate(Ram *ram);
void ram_free(void *ptr, Ram *ram);

#endif