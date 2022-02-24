#include "ram.h"
#include <cpu.h>
#include <dev/bus.h>
#include <ivt.h>
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

        ram_write(cpu->SP - 1, MEM_BYTE, a[0], cpu->ram);
        ram_write(cpu->SP - 2, MEM_BYTE, a[1], cpu->ram);
        ram_write(cpu->SP - 3, MEM_BYTE, a[2], cpu->ram);
        ram_write(cpu->SP - 4, MEM_BYTE, a[3], cpu->ram);
    }

    else
        ram_write(cpu->SP - 4, MEM_BYTE, what, cpu->ram);
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

    uint32_t ret;

    ram_read(cpu->PC, MEM_4_BYTES, &ret, cpu->ram);

    cpu->PC += 4;

    return ret;
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

static uint32_t imm_1_byte(uint32_t *size, Cpu *cpu)
{

    uint8_t ret = fetch(cpu);

    if (size)
    {
        *size = 3;
    }
    return ret;
}

static uint32_t imm_2_bytes(uint32_t *size, Cpu *cpu)
{

    (void)fetch16;

    uint8_t bytes[] = {fetch(cpu), fetch(cpu)};

    if (size)
    {
        *size = 4;
    }

    return *(uint16_t *)bytes;
}

static uint32_t imm_4_bytes(uint32_t *size, Cpu *cpu)
{

    uint32_t ret = fetch32(cpu);
    if (size)
    {
        *size = 6;
    }

    (void)u8_to_u32;
    return ret;
}
uint32_t (*imm_funcs[])(uint32_t *, Cpu *cpu) = {
    [0x2A] = imm_1_byte,
    [0x2B] = imm_2_bytes,
    [0x2D] = imm_4_bytes,
};

uint32_t handle_imm(uint8_t sspec, uint32_t *size, Cpu *cpu)
{
    return imm_funcs[sspec](size, cpu);
}

static uint32_t do_byte_imm(uint32_t *size, Cpu *cpu)
{
    uint8_t sspec = fetch(cpu);

    return handle_imm(sspec, size, cpu);
}

static uint32_t reg_a(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(A); }
static uint32_t reg_b(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(B); }
static uint32_t reg_c(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(C); }
static uint32_t reg_d(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(D); }
static uint32_t reg_e(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(E); }
static uint32_t reg_f(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(F); }
static uint32_t reg_g(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(G); }
static uint32_t reg_h(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(H); }
static uint32_t reg_pc(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(PC); }
static uint32_t reg_sp(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(SP); }
static uint32_t reg_ivt(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(IVT); }
static uint32_t reg_pt(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(PT); }
static uint32_t reg_pl(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(PL); }

static uint32_t reg_pf(uint32_t *size, Cpu *cpu) { CPU_REG_STUB(PF); }
static uint32_t deref(uint32_t *size, Cpu *cpu)
{
    *size = 2;
    (void)size;

    uint32_t ret = 0;

    bus_read(fetch32(cpu), MEM_4_BYTES, &ret, cpu->ram, cpu->bus);

    return ret;
}

static uint32_t (*funcs[])(uint32_t *, Cpu *) = {
    [0x1a] = do_byte_imm,
    [0x1b] = reg_a,
    [0x1c] = reg_b,
    [0x1d] = reg_c,
    [0x1e] = reg_d,
    [0x1f] = reg_e,
    [0x20] = reg_f,
    [0x21] = reg_g,
    [0x22] = reg_h,
    [0x23] = reg_pc,
    [0x24] = reg_sp,
    [0x25] = reg_ivt,
    [0x26] = reg_pt,
    [0x27] = reg_pl,
    [0x28] = reg_pf,
    [0x29] = deref,
};

uint32_t get_val_from_special_byte(uint32_t *size, Cpu *cpu)
{

    uint32_t specifier = fetch(cpu);

    if (specifier < 0x1a || specifier > 0x29)
    {
        ivt_trigger_interrupt(0, true, cpu);
    }

    else
        return funcs[specifier](size, cpu);

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

    case 0x25:
    {
        if (cpu->PL == 0)
        {
            ivt_set(val, cpu->ram);
            CPU_REG_STUB(IVT);
        }
        else
        {
            ivt_trigger_interrupt(2, false, cpu);
        }
        break;
    }

    case 0x26:
    {
        if (cpu->PL == 0)
        {
            CPU_REG_STUB(PT);
        }
        else
        {
            ivt_trigger_interrupt(2, false, cpu);
        }
        break;
    }

    case 0x27:
    {
        if (cpu->PL == 0)
        {
            CPU_REG_STUB(PL);
        }
        else
        {
            ivt_trigger_interrupt(2, false, cpu);
        }
        break;
    }

    case 0x28:
    {
        if (cpu->PF == 0)
        {
            CPU_REG_STUB(PF);
        }
        break;
    }
    }
}

void pop_from_special_byte(Cpu *cpu)
{
    set_from_special_byte(pop(cpu), cpu);
}
