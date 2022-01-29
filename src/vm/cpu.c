#include "dev/io.h"
#include "ram.h"
#include <cpu.h>
#include <dev/bus.h>
#include <lib/log.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

#define CPU_OP(OP)                                              \
    uint32_t start_pc = cpu->PC;                                \
    (void)start_pc;                                             \
    uint32_t inst_size = 0;                                     \
    uint32_t op_a = get_val_from_special_byte(&inst_size, cpu); \
    uint32_t prev_pc = cpu->PC += inst_size;                    \
    uint32_t op_b = get_val_from_special_byte(&inst_size, cpu); \
    uint32_t new_prev = prev_pc + inst_size;                    \
    cpu->PC = start_pc;                                         \
    set_from_special_byte(op_a OP op_b, cpu);                   \
    cpu->PC = new_prev;

#define CPU_SOP(OP)                                             \
    uint32_t start_pc = cpu->PC;                                \
    (void)start_pc;                                             \
    uint32_t inst_size = 0;                                     \
    get_val_from_special_byte(&inst_size, cpu);                 \
    uint32_t prev_pc = cpu->PC += inst_size;                    \
    uint32_t op_b = get_val_from_special_byte(&inst_size, cpu); \
    cpu->PC = start_pc;                                         \
    set_from_special_byte(OP op_b, cpu);                        \
    cpu->PC = prev_pc;

#define CPU_CELL_OP(TYPE)                                                \
    uint32_t start_pc = cpu->PC;                                         \
    (void)start_pc;                                                      \
    uint32_t inst_size = 0;                                              \
    get_val_from_special_byte(&inst_size, cpu);                          \
    uint32_t prev_pc = cpu->PC += inst_size;                             \
    uint32_t rhs = get_val_from_special_byte(&inst_size, cpu);           \
    uint32_t new_prev = prev_pc + inst_size;                             \
    cpu->PC = start_pc;                                                  \
    Cons *buffer = (Cons *)((uint64_t)rhs + (uint64_t)cpu->ram->buffer); \
    set_from_special_byte(buffer->TYPE, cpu);                            \
    cpu->PC = new_prev;

#define CPU_GET_LHS_AND_RHS()                                  \
    uint32_t start_pc = cpu->PC;                               \
    (void)start_pc;                                            \
    uint32_t inst_size = 0;                                    \
    uint32_t lhs = get_val_from_special_byte(&inst_size, cpu); \
    uint32_t prev_pc = cpu->PC;                                \
    uint32_t rhs = get_val_from_special_byte(&inst_size, cpu); \
    uint32_t new_prev = prev_pc + inst_size;                   \
    cpu->PC = new_prev;

void cpu_init(Ram *ram, Cpu *cpu, size_t rom_size)
{
    cpu->ram = ram;
    cpu->SP = MEMORY_SIZE;
    cpu->PC = 0x1000;
    cpu->rom_size = rom_size;
    cpu->flags.EQ = 0;
    cpu->flags.LT = 0;
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

            cpu->PC = start_pc;

            set_from_special_byte((uint64_t)buffer - (uint64_t)cpu->ram->buffer, cpu);

            cpu->PC = new_prev + 1;

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

            bus_read(rhs, MEM_BYTE, &value, cpu->ram);

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
            uint32_t val = get_val_from_special_byte(NULL, cpu);

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
            uint32_t addr = get_val_from_special_byte(NULL, cpu);
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

            uint32_t addr = get_val_from_special_byte(NULL, cpu);

            if (cpu->flags.EQ == 1)
                cpu->PC = addr;

            break;
        }

        case 0x14: // JNE
        {
            uint32_t addr = get_val_from_special_byte(NULL, cpu);

            if (cpu->flags.EQ == 0)
                cpu->PC = addr;
            break;
        }

        case 0x15: // JLT
        {
            uint32_t addr = get_val_from_special_byte(NULL, cpu);

            if (cpu->flags.LT == 1)
                cpu->PC = addr;

            break;
        }

        case 0x16: // JGT
        {
            uint32_t addr = get_val_from_special_byte(NULL, cpu);

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

            io_write(lhs, rhs);

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
