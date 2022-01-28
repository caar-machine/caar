#ifndef CAAR_CPU_H
#define CAAR_CPU_H

#include <ram.h>
#include <stddef.h>

#define STACK_SIZE 8192

typedef struct
{
    uint32_t A, B, C, D, E, F, G, H, SP, PC;

    struct
    {
<<<<<<< HEAD
        uint8_t EQ : 1;
        uint8_t LT : 1;
=======
      uint8_t EQ : 1;
			uint8_t LT : 1;
>>>>>>> 1f34b3cf0a7af70929583035d861757db7a6e4ea
    } flags;

    Ram *ram;
    size_t rom_size;
} Cpu;

typedef struct __attribute__((packed))
{
    uint32_t car;
    uint32_t cdr;
} Cons;

void cpu_init(Ram *ram, Cpu *cpu, size_t rom_size);
void cpu_do_cycle(Cpu *cpu);

#endif