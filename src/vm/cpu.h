#ifndef CAAR_CPU_H
#define CAAR_CPU_H

#include <dev/bus.h>
#include <ram.h>
#include <stddef.h>

#define STACK_SIZE 8192

typedef struct
{
    uint32_t A, B, C, D, E, F, G, H, SP, PC, IVT, PT, PF;
    uint8_t PL : 1;

    struct
    {
        uint8_t EQ : 1;
        uint8_t LT : 1;
    } flags;

    Ram *ram;
    Bus *bus;
    size_t rom_size;
} Cpu;

typedef struct __attribute__((packed))
{
    uint32_t car;
    uint32_t cdr;
} Cons;

void cpu_init(Ram *ram, Bus *bus, Cpu *cpu, size_t rom_size);
void cpu_do_cycle(Cpu *cpu);

#endif