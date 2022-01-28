#include "ram.h"
#include <cpu.h>
#include <lib/log.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define CPU_OP(OP)                                     \
    uint32_t op_a = get_val_from_special_byte(cpu);    \
    uint32_t prev_pc = cpu->PC++;                      \
    uint32_t op_b = get_val_from_special_byte(cpu);    \
    uint32_t new_prev = cpu->PC + (cpu->PC - prev_pc); \
    cpu->PC -= (cpu->PC - prev_pc);                    \
    set_from_special_byte(op_a OP op_b, cpu);          \
    cpu->PC = new_prev;

#define CPU_SOP(OP)                                 \
    get_val_from_special_byte(cpu);                 \
    uint32_t prev_pc = cpu->PC++;                   \
    uint32_t op_b = get_val_from_special_byte(cpu); \
    cpu->PC -= (cpu->PC - prev_pc);                 \
    set_from_special_byte(OP op_b, cpu);

#define CPU_CELL_OP(TYPE)                                                \
    get_val_from_special_byte(cpu);                                      \
    uint32_t prev_pc = cpu->PC++;                                        \
    uint32_t rhs = get_val_from_special_byte(cpu);                       \
    uint32_t new_prev = cpu->PC + (cpu->PC - prev_pc);                   \
    cpu->PC -= (cpu->PC - prev_pc);                                      \
    Cons *buffer = (Cons *)((uint64_t)rhs + (uint64_t)cpu->ram->buffer); \
    set_from_special_byte(buffer->TYPE, cpu);                            \
    cpu->PC = new_prev;

#define CPU_GET_LHS_AND_RHS()                          \
    uint32_t lhs = get_val_from_special_byte(cpu);     \
    uint32_t prev_pc = cpu->PC++;                      \
    uint32_t rhs = get_val_from_special_byte(cpu);     \
    uint32_t new_prev = cpu->PC + (cpu->PC - prev_pc); \
    cpu->PC -= (cpu->PC - prev_pc);

void cpu_init(Ram *ram, Cpu *cpu, size_t rom_size)
{
    cpu->ram = ram;
    cpu->SP = MEMORY_SIZE;
    cpu->PC = 0x1000;
    cpu->rom_size = rom_size;
    cpu->flags.EQ = 0;
    cpu->flags.LT = 0;
}

static uint32_t u8_to_u32(const uint8_t *bytes)
{
    uint32_t u32 = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
    return u32;
}

static void u32_to_u8(const uint32_t u32, uint8_t *u8)
{
    u8[0] = (u32 & 0xff000000) >> 24;
    u8[1] = (u32 & 0x00ff0000) >> 16;
    u8[2] = (u32 & 0x0000ff00) >> 8;
    u8[3] = u32 & 0x000000ff;
}

static uint8_t fetch(Cpu *cpu)
{
    uint32_t value = 0;
    ram_read(cpu->PC++, MEM_BYTE, &value, cpu->ram);

    return value;
}

static void push(uint32_t what, Cpu *cpu)
{
    cpu->SP -= 4;

    if ((cpu->SP - STACK_SIZE) == 0)
    {
        printf("\033[1;31mERROR:\033[0m stack overflow\n");
    }

    if (what > 255)
    {
        uint8_t a[4];
        u32_to_u8(what, a);

        ram_write(cpu->SP, a[0], cpu->ram);
        ram_write(cpu->SP - 1, a[1], cpu->ram);
        ram_write(cpu->SP - 2, a[2], cpu->ram);
        ram_write(cpu->SP - 3, a[3], cpu->ram);
    }

    else
        ram_write(cpu->SP, what, cpu->ram);
}

static uint32_t pop(Cpu *cpu)
{

    if (cpu->SP > MEMORY_SIZE)
    {
        printf("\033[1;31mERROR:\033[0m pop() out of bounds\n");
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

#define CPU_REG_STUB(reg) \
    cpu->PC -= 1;         \
    return cpu->reg;

static uint32_t get_val_from_special_byte(Cpu *cpu)
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
            cpu->PC -= 2;

            return ret;
        }
        // 4 bytes
        else if (sspec == 0x29)
        {
            uint32_t ret = fetch32(cpu);
            cpu->PC -= 5;

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
    }

    return 0;
}

static uint32_t pop32(Cpu *cpu)
{
    uint8_t a[4] = {pop(cpu), pop(cpu), pop(cpu), pop(cpu)};

    return u8_to_u32(a);
}

#undef CPU_REG_STUB

#define CPU_REG_STUB(reg) \
    cpu->reg = val;       \
    break;

static void set_from_special_byte(uint32_t val, Cpu *cpu)
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

static void pop_from_special_byte(Cpu *cpu)
{

    uint32_t specifier = fetch(cpu);

    switch (specifier)
    {
    case 0x1b: // Register A
    {
        uint8_t sspec = fetch(cpu);

        uint32_t val = 0;

        // 1 byte
        if (sspec == 0x27)
        {
            val = pop(cpu);
        }

        // 4 bytes
        else if (sspec == 0x30)
        {
            val = pop32(cpu);
        }

        cpu->A = val;

        break;
    }
    }
}

void cpu_do_cycle(Cpu *cpu)
{
    if (cpu->PC - 0x1000 < cpu->rom_size)
    {
        uint8_t opcode = fetch(cpu);

        switch (opcode)
        {

        case 0x00: // cons
        {
            CPU_GET_LHS_AND_RHS();

            Cons *buffer = ram_allocate(cpu->ram);

            buffer->car = lhs;
            buffer->cdr = rhs;

            set_from_special_byte((uint64_t)buffer - (uint64_t)cpu->ram->buffer, cpu);

            cpu->PC = new_prev;

            break;
        }

        case 0x1: // car
        {
            CPU_CELL_OP(car);
            break;
        }

        case 0x2: // cdr
        {
            CPU_CELL_OP(cdr);
            break;
        }

        case 0x3: // nop
        {
            break;
        }

        case 0x4: // LDR
        {
            CPU_GET_LHS_AND_RHS();

            (void)lhs;

            uint32_t value = 0;
            ram_read(rhs, MEM_BYTE, &value, cpu->ram);

            set_from_special_byte(value, cpu);

            cpu->PC = new_prev;

            break;
        }

        case 0x5: // STR
        {
            CPU_GET_LHS_AND_RHS();

            ram_write(lhs, rhs, cpu->ram);

            set_from_special_byte(0, cpu);

            cpu->PC = new_prev;

            break;
        }

        case 0x6: // Add
        {

            CPU_OP(+);

            break;
        }

        case 0x7: // sub
        {
            CPU_OP(-);
            break;
        }

        case 0x8: // div
        {
            CPU_OP(/);
            break;
        }

        case 0x9: // mul
        {
            CPU_OP(*);
            break;
        }

        case 0xA: // mod
        {
            CPU_OP(%);
            break;
        }

        case 0xB: // not
        {
            CPU_SOP(~);

            break;
        }

        case 0xC: // and
        {
            CPU_OP(&);
            break;
        }

        case 0xD: // or
        {
            CPU_OP(|);
            break;
        }

        case 0xE: // xor
        {
            CPU_OP(^);
            break;
        }

        case 0xF: // push
        {
            uint32_t prev_pc = cpu->PC;
            uint32_t val = get_val_from_special_byte(cpu);

            push(val, cpu);

            cpu->PC += (cpu->PC - prev_pc);
            cpu->PC++;

            break;
        }

        case 0x10: // pop
        {
            pop_from_special_byte(cpu);
            break;
        }

        case 0x11: // JMP
        {
            uint32_t addr = get_val_from_special_byte(cpu);
            cpu->PC = addr;
            break;
        }

        case 0x12: // CMP
        {
            CPU_GET_LHS_AND_RHS();

            if (lhs == rhs)
            {
                cpu->flags.EQ = 1;
            }
            else
            {
                cpu->flags.EQ = 0;
            }

            if (lhs < rhs)
            {
                cpu->flags.LT = 1;
            }
            else
            {
                cpu->flags.LT = 0;
            }

            cpu->PC = new_prev;
            break;
        }

        case 0x13: // JE
        {

            uint32_t addr = get_val_from_special_byte(cpu);

            if (cpu->flags.EQ == 1)
                cpu->PC = addr;

            break;
        }

        case 0x14: // JNE
        {
            uint32_t addr = get_val_from_special_byte(cpu);

            if (cpu->flags.EQ == 0)
                cpu->PC = addr;
            break;
        }

        case 0x15: // JLT
        {
            uint32_t addr = get_val_from_special_byte(cpu);

            if (cpu->flags.LT == 1)
                cpu->PC = addr;

            break;
        }

        case 0x16: // JGT
        {
            uint32_t addr = get_val_from_special_byte(cpu);

            if (cpu->flags.LT == 0 && cpu->flags.EQ == 0)
                cpu->PC = addr;

            break;
        }

        case 0x17: // IN
        {
            error("TODO: in");
            break;
        }

        case 0x18: // OUT
        {
            CPU_GET_LHS_AND_RHS();

            if (lhs == 1)
                putchar(rhs);

            cpu->PC = new_prev;
            break;
        }

        default:
        {
            warn("invalid opcode: %x", opcode);
            break;
        }
        }
    }

    else
    {
        free(cpu->ram->buffer);
        exit(0);
    }
}
