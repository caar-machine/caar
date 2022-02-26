#ifndef CAAR_CPU_H
#define CAAR_CPU_H

#include <dev/bus.h>
#include <ram.h>
#include <stddef.h>

#define STACK_SIZE 8192

#define CPU_R0 0
#define CPU_R1 1
#define CPU_R2 2
#define CPU_R3 3
#define CPU_R4 4
#define CPU_R5 5
#define CPU_R6 6
#define CPU_R7 7
#define CPU_SP 8
#define CPU_PC 9
#define CPU_IVT 10
#define CPU_PT 11
#define CPU_PF 12

typedef struct __attribute__((packed))
{
    uint8_t param_type : 2;
    uint8_t value : 6;
} InstructionEncoding;

typedef struct
{
    enum
    {
        DECODE_IMM,
        DECODE_REG,
    } type;
    union
    {
        uint32_t *ptr;
        uint32_t val;
    };

} InstructionDecoding;

typedef struct
{
    uint32_t regs[14];

    struct
    {
        uint8_t EQ : 1;
        uint8_t LT : 1;
        uint8_t PL : 1;
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

void push(uint32_t what, Cpu *cpu);

uint32_t pop(Cpu *cpu);

#endif
