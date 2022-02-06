#ifndef CAAR_IVT_H
#define CAAR_IVT_H
#include <cpu.h>
#include <stdbool.h>

typedef struct __attribute__((packed))
{
    uint8_t used : 1;
    uint8_t dpl : 2;
    uint32_t address;
} IvtEntry;

typedef struct __attribute__((packed))
{
    IvtEntry entries[256];
} Ivt;

void ivt_set(uint32_t address, Ram *ram);
void ivt_trigger_interrupt(int interrupt, bool software, Cpu *cpu);
#endif