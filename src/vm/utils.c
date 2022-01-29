#include "ram.h"
#include <cpu.h>
#include <dev/bus.h>
#include <lib/log.h>
#include <stdint.h>

static uint32_t u8_to_u32(const uint8_t *bytes)
{
    uint32_t u32 = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
    return u32;
}

static uint16_t u8_to_u16(const uint8_t *bytes)
{
    uint16_t u16 = (bytes[0] << 8) + bytes[1];
    return u16;
}

static void u32_to_u8(const uint32_t u32, uint8_t *u8)
{
    u8[0] = (u32 & 0xff000000) >> 24;
    u8[1] = (u32 & 0x00ff0000) >> 16;
    u8[2] = (u32 & 0x0000ff00) >> 8;
    u8[3] = u32 & 0x000000ff;
}

uint8_t fetch(Cpu *cpu)
{
    uint32_t value = 0;
    ram_read(cpu->PC++, MEM_BYTE, &value, cpu->ram);

    return value;
}

void push(uint32_t what, Cpu *cpu)
{
    cpu->SP -= 4;

    if (cpu->SP == STACK_SIZE)
    {
        error("stack overflow");
    }

    if (what > 255)
    {
        uint8_t a[4];
        u32_to_u8(what, a);

        ram_write(cpu->SP - 1, a[0], cpu->ram);
        ram_write(cpu->SP - 2, a[1], cpu->ram);
        ram_write(cpu->SP - 3, a[2], cpu->ram);
        ram_write(cpu->SP - 4, a[3], cpu->ram);
    }

    else
        ram_write(cpu->SP - 4, what, cpu->ram);
}

uint32_t pop(Cpu *cpu)
{

    if (cpu->SP > MEMORY_SIZE)
    {
        error("pop() out of bounds");
        return 0;
    }

    uint32_t ret = 0;
    ram_read(cpu->SP - 4, MEM_4_BYTES, &ret, cpu->ram);

    cpu->SP += 4;

    return ret;
}

static uint32_t fetch32(Cpu *cpu)
{
    uint8_t a[4] = {fetch(cpu), fetch(cpu), fetch(cpu), fetch(cpu)};

    return u8_to_u32(a);
}

static uint16_t fetch16(Cpu *cpu)
{
    uint8_t a[2] = {fetch(cpu), fetch(cpu)};
    return u8_to_u16(a);
}

#define CPU_REG_STUB(reg) \
    if (size)             \
        *size = 0;        \
    return cpu->reg;

uint32_t get_val_from_special_byte(uint32_t *size, Cpu *cpu)
{

    uint32_t specifier = fetch(cpu);

    switch (specifier)
    {
    // Immediate value
    case 0x1a:
    {
        uint8_t sspec = fetch(cpu);

        // 1 byte
        if (sspec == 0x26)
        {
            uint8_t ret = fetch(cpu);

            if (size)
                *size = 3;
            return ret;
        }

        // 2 bytes
        if (sspec == 0x27)
        {
            uint16_t ret = fetch16(cpu);

            if (size)
                *size = 4;

            return ret;
        }

        // 4 bytes
        else if (sspec == 0x29)
        {
            uint32_t ret = fetch32(cpu);
            if (size)
                *size = 6;
            return ret;
        }

        break;
    }

    case 0x1b:
    {
        CPU_REG_STUB(A);
    }

    case 0x1c:
    {
        CPU_REG_STUB(B);
    }
    case 0x1d:
    {
        CPU_REG_STUB(C);
    }
    case 0x1e:
    {
        CPU_REG_STUB(D);
    }

    case 0x1f:
    {
        CPU_REG_STUB(E);
    }

    case 0x20:
    {
        CPU_REG_STUB(F);
    }

    case 0x21:
    {
        CPU_REG_STUB(G);
    }

    case 0x22:
    {
        CPU_REG_STUB(H);
    }

    case 0x23:
    {
        CPU_REG_STUB(PC);
    }

    case 0x24:
    {
        CPU_REG_STUB(SP);
    }

    case 0x25:
    {
        uint32_t ret = 0;

        bus_read(fetch32(cpu), MEM_4_BYTES, &ret, cpu->ram);

        return ret;
    }

    default:
        error("Unknown specifier: %x at PC=%x", specifier, cpu->PC);
        break;
    }

    return 0;
}

#undef CPU_REG_STUB

#define CPU_REG_STUB(reg) \
    cpu->reg = val;       \
    break;

void set_from_special_byte(uint32_t val, Cpu *cpu)
{
    uint32_t specifier = fetch(cpu);

    switch (specifier)
    {
    case 0x1b:
    {
        CPU_REG_STUB(A);
    }

    case 0x1c:
    {
        CPU_REG_STUB(B);
    }
    case 0x1d:
    {
        CPU_REG_STUB(C);
    }
    case 0x1e:
    {
        CPU_REG_STUB(D);
    }

    case 0x1f:
    {
        CPU_REG_STUB(E);
    }

    case 0x20:
    {
        CPU_REG_STUB(F);
    }

    case 0x21:
    {
        CPU_REG_STUB(G);
    }

    case 0x22:
    {
        CPU_REG_STUB(H);
    }

    case 0x23:
    {
        CPU_REG_STUB(PC);
    }

    case 0x24:
    {
        CPU_REG_STUB(SP);
    }
    }
}

void pop_from_special_byte(Cpu *cpu)
{
    set_from_special_byte(pop(cpu), cpu);
}
