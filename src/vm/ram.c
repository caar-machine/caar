#include <ram.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCKS_COUNT 4096

uint8_t *free_blocks[BLOCKS_COUNT];
size_t free_blocks_ptr = BLOCKS_COUNT - 1;

void ram_init(Ram *ram)
{
    ram->buffer = malloc(MEMORY_SIZE);

    memset(ram->buffer, 0, MEMORY_SIZE);

    for (size_t i = 0; i < BLOCKS_COUNT; i++)
    {
        free_blocks[i] = (uint8_t *)(BLOCKS_COUNT - i - 1);
    }

    ram->allocator_index = 4095;
}

void ram_write(uint32_t addr, uint32_t data, Ram *ram)
{
    ram->buffer[addr] = data;
}

uint8_t ram_read(uint32_t addr, Ram *ram)
{
    return ram->buffer[addr];
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
