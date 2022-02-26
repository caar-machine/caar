#include <lib/log.h>
#include <ram.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define BLOCKS_COUNT 4096

uint8_t *free_blocks[BLOCKS_COUNT];
size_t free_blocks_ptr = BLOCKS_COUNT - 1;

void ram_init(Ram *ram)
{
    ram->buffer = calloc(MEMORY_SIZE, sizeof(uint8_t));

    for (size_t i = 0; i < BLOCKS_COUNT; i++)
    {
        free_blocks[i] = (uint8_t *)(BLOCKS_COUNT - i - 1);
    }

    ram->allocator_index = 4095;
}

void ram_write(uint32_t addr, MemSize size, uint32_t data, Ram *ram)
{
    if (size == MEM_BYTE)
        ram->buffer[addr] = data;
    if (size == MEM_4_BYTES)
    {
        uint8_t *bytes = (uint8_t *)&data;
        ram->buffer[addr] = bytes[0];
        ram->buffer[addr + 1] = bytes[1];
        ram->buffer[addr + 2] = bytes[2];
        ram->buffer[addr + 3] = bytes[3];
    }
}

void ram_read(uint32_t addr, MemSize size, uint32_t *value, Ram *ram)
{
    void *mem = ram->buffer + addr;

    switch (size)
    {
    case MEM_BYTE:
        *value = *(uint8_t *)mem;
        break;
    case MEM_2_BYTES:
        *value = *(uint16_t *)mem;
        break;
    case MEM_4_BYTES:
        *value = *(uint32_t *)mem;
        break;
    }
}

void *ram_allocate(Ram *ram)
{
    void *ret = free_blocks[free_blocks_ptr];

    free_blocks_ptr -= 1;

    return (void *)(ram->buffer + (uint64_t)ret);
}

void ram_free(void *ptr, Ram *ram)
{
    free_blocks_ptr += 1;

    free_blocks[free_blocks_ptr] = (void *)((uint64_t)ptr - (uint64_t)ram->buffer);
}
