#include "utils.h"
#include <ivt.h>
#include <stdlib.h>

static Ivt ivt = {0};

void ivt_set(uint32_t address, Ram *ram)
{
    uint8_t bytes[sizeof(Ivt)] = {0};

    for (size_t i = 0; i < sizeof(Ivt); i++)
    {
        ram_read(address + i, MEM_BYTE, (uint32_t *)&bytes[i], ram);
    }

    ivt = *(Ivt *)bytes;
}

void ivt_trigger_interrupt(int interrupt, bool software, Cpu *cpu)
{
    push(cpu->PC, cpu);

    if (software && ivt.entries[interrupt].dpl < cpu->PL)
    {
        error("TODO: cpu faults");
    }

    if (ivt.entries[interrupt].used)
    {
        cpu->PC = ivt.entries[interrupt].address;
    }

    else
    {
        warn("Unhandled interrupt: %d", interrupt);

        cpu->PC = pop(cpu);
    }
}
