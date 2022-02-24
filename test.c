#include "dev/io.h"
#include "ram.h"
#include <cpu.h>
#include <dev/bus.h>
#include <ivt.h>
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
    uint32_t op_b = get_val_from_special_byte(&inst_size, cpu); \
    uint32_t new_prev = cpu->PC;                                \
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
    uint32_t new_prev = cpu->PC;                               \
    (void)prev_pc;                                             \
    (void)new_prev;

void cpu_init(Ram *ram, Bus *bus, Cpu *cpu, size_t rom_size)
{
    cpu->ram = ram;
    cpu->bus = bus;
    cpu->SP = MEMORY_SIZE;
    cpu->PC = 0x1000;
    cpu->rom_size = rom_size;
    cpu->flags.EQ = 0;
    cpu->flags.LT = 0;
    cpu->PL = 0;
}

void cpu_do_cycle(Cpu *cpu)
{
    static const void *ops[] = {
        [0x0] = &&cons,
        [0x1] = &&car,
        [0x2] = &&cdr,
        [0x3] = &&nop,
        [0x4] = &&ldr,
        [0x5] = &&str,
        [0x6] = &&add,
        [0x7] = &&sub,
        [0x8] = &&div,
        [0x9] = &&mul,
        [0xA] = &&mod,
        [0xB] = &&not,
        [0xC] = &&and,
        [0xD] = && or
        ,
        [0xE] = &&xor,
        [0xF] = &&push,
        [0x10] = &&pop,
        [0x11] = &&jmp,
        [0x12] = &&cmp,
        [0x13] = &&je,
        [0x14] = &&jne,
        [0x15] = &&jlt,
        [0x16] = &&jgt,
        [0x17] = &&in,
        [0x18] = &&out,
        [0x19] = &&stw,
        [0x1A] = &&ldw,
        [0x1B] = &&int_,

    };

    uint8_t opcode = fetch(cpu);

    if (opcode > 0x1B)
    {
        ivt_trigger_interrupt(0, false, cpu);
        return;
    }

    goto *ops[opcode];

cons:
{
    CPU_GET_LHS_AND_RHS();

    Cons *buffer = ram_allocate(cpu->ram);

    buffer->car = lhs;
    buffer->cdr = rhs;

    cpu->PC = start_pc;

    set_from_special_byte((uint64_t)buffer - (uint64_t)cpu->ram->buffer, cpu);

    cpu->PC = new_prev + 1;
};

car: // car
{
    CPU_CELL_OP(car);
};

cdr: // cdr
{
    CPU_CELL_OP(cdr);
};

nop: // nop
{
};

ldr: // LDR
{
    CPU_GET_LHS_AND_RHS();

    (void)lhs;

    uint32_t value = 0;

    bus_read(rhs, MEM_BYTE, &value, cpu->ram, cpu->bus);

    cpu->PC = start_pc;

    set_from_special_byte(value, cpu);

    cpu->PC = new_prev;
};

str: // STR
{
    CPU_GET_LHS_AND_RHS();

    bus_write(lhs, rhs, MEM_BYTE, cpu->ram, cpu->bus);
};

add: // Add
{

    CPU_OP(+);
};

sub: // sub
{
    CPU_OP(-);
};

div: // div
{
    CPU_OP(/);
};

mul: // mul
{
    CPU_OP(*);
};

mod: // mod
{
    CPU_OP(%);
};

    not: // not
    {
        CPU_SOP(~);
    };

    and: // and
    {
        CPU_OP(&);
    };

    or: // or
    {
        CPU_OP(|);
    };

    xor: // xor
    {
        CPU_OP(^);
    };

push: // push
{
    uint32_t inst_size = 0;
    uint32_t val = get_val_from_special_byte(&inst_size, cpu);

    push(val, cpu);
};

pop: // pop
{
    pop_from_special_byte(cpu);
};

jmp: // JMP
{
    info("we do a little jumping");
    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    cpu->PC = addr;
};

cmp: // CMP
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
};

je: // JE
{

    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    if (cpu->flags.EQ == 1)
        cpu->PC = addr;
};

jne: // JNE
{
    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    if (cpu->flags.EQ == 0)
        cpu->PC = addr;
};

jlt: // JLT
{
    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    if (cpu->flags.LT == 1)
        cpu->PC = addr;
};

jgt: // JGT
{
    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    if (cpu->flags.LT == 0 && cpu->flags.EQ == 0)
        cpu->PC = addr;
};

in: // IN
{
    CPU_GET_LHS_AND_RHS();

    (void)lhs;

    cpu->PC = start_pc;

    set_from_special_byte(io_read(rhs), cpu);

    cpu->PC = new_prev;
};

out: // OUT
{

    CPU_GET_LHS_AND_RHS();

    io_write(lhs, rhs);
};

stw: // STW
{
    CPU_GET_LHS_AND_RHS();

    bus_write(lhs, MEM_4_BYTES, rhs, cpu->ram, cpu->bus);
};

ldw: // LDW
{
    CPU_GET_LHS_AND_RHS();

    (void)lhs;

    uint32_t value = 0;

    bus_read(rhs, MEM_4_BYTES, &value, cpu->ram, cpu->bus);

    cpu->PC = start_pc;

    set_from_special_byte(value, cpu);

    cpu->PC = new_prev;
};

int_: // Int
{
    uint32_t val = get_val_from_special_byte(NULL, cpu);

    ivt_trigger_interrupt(val, true, cpu);
};
}
