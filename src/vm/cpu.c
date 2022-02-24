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

void cons(Cpu *cpu)
{
    CPU_GET_LHS_AND_RHS();

    Cons *buffer = ram_allocate(cpu->ram);

    buffer->car = lhs;
    buffer->cdr = rhs;

    cpu->PC = start_pc;

    set_from_special_byte((uint64_t)buffer - (uint64_t)cpu->ram->buffer, cpu);

    cpu->PC = new_prev + 1;
}

void car(Cpu *cpu)
{
    CPU_CELL_OP(car);
}

void cdr(Cpu *cpu)
{
    CPU_CELL_OP(cdr);
}

void nop(Cpu *cpu)
{
    (void)cpu;
}

void ldr(Cpu *cpu)
{

    CPU_GET_LHS_AND_RHS();

    (void)lhs;

    uint32_t value = 0;

    bus_read(rhs, MEM_BYTE, &value, cpu->ram, cpu->bus);

    cpu->PC = start_pc;

    set_from_special_byte(value, cpu);

    cpu->PC = new_prev;
}

void str(Cpu *cpu)
{
    CPU_GET_LHS_AND_RHS();

    bus_write(lhs, rhs, MEM_BYTE, cpu->ram, cpu->bus);
}

void add(Cpu *cpu)
{
    CPU_OP(+);
}

void sub(Cpu *cpu)
{
    CPU_OP(-);
}

void _div(Cpu *cpu)
{
    CPU_OP(/);
}

void mul(Cpu *cpu)
{
    CPU_OP(*);
}

void mod(Cpu *cpu)
{
    CPU_OP(%);
}

void not(Cpu * cpu)
{
    CPU_SOP(~);
}

void and (Cpu * cpu)
{
    CPU_OP(&);
}

void _or(Cpu *cpu)
{
    CPU_OP(|);
}

void _xor(Cpu *cpu)
{
    CPU_OP(^);
}

void _push(Cpu *cpu)
{
    uint32_t inst_size = 0;
    uint32_t val = get_val_from_special_byte(&inst_size, cpu);

    push(val, cpu);
}

void _pop(Cpu *cpu)
{
    pop_from_special_byte(cpu);
}

void jmp(Cpu *cpu)
{

    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    cpu->PC = addr;
}

void cmp(Cpu *cpu)
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
}

void je(Cpu *cpu)
{

    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    if (cpu->flags.EQ == 1)
        cpu->PC = addr;
}

void jne(Cpu *cpu)
{

    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    if (cpu->flags.EQ == 0)
        cpu->PC = addr;
}

void jlt(Cpu *cpu)
{
    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    if (cpu->flags.LT == 1)
        cpu->PC = addr;
}

void jgt(Cpu *cpu)
{
    uint32_t addr = get_val_from_special_byte(NULL, cpu);

    if (cpu->flags.LT == 0 && cpu->flags.EQ == 0)
        cpu->PC = addr;
}

void in(Cpu *cpu)
{
    CPU_GET_LHS_AND_RHS();

    (void)lhs;

    cpu->PC = start_pc;

    set_from_special_byte(io_read(rhs), cpu);

    cpu->PC = new_prev;
}

void out(Cpu *cpu)
{
    CPU_GET_LHS_AND_RHS();

    io_write(lhs, rhs);
}

void stw(Cpu *cpu)
{

    CPU_GET_LHS_AND_RHS();

    bus_write(lhs, MEM_4_BYTES, rhs, cpu->ram, cpu->bus);
}

void ldw(Cpu *cpu)
{
    CPU_GET_LHS_AND_RHS();

    (void)lhs;

    uint32_t value = 0;

    bus_read(rhs, MEM_4_BYTES, &value, cpu->ram, cpu->bus);

    cpu->PC = start_pc;

    set_from_special_byte(value, cpu);

    cpu->PC = new_prev;
}

void _int(Cpu *cpu)
{
    uint32_t val = get_val_from_special_byte(NULL, cpu);
    ivt_trigger_interrupt(val, true, cpu);
}

static void (*ops[])(Cpu *cpu) = {
    [0] = cons,
    [1] = car,
    [2] = cdr,
    [3] = nop,
    [4] = ldr,
    [5] = str,
    [6] = add,
    [7] = sub,
    [8] = _div,
    [9] = mul,
    [0xA] = mod,
    [0xB] = not,
    [0xC] = and,
    [0xD] = _or,
    [0xE] = _xor,
    [0xF] = _push,
    [0x10] = _pop,
    [0x11] = jmp,
    [0x12] = cmp,
    [0x13] = je,
    [0x14] = jne,
    [0x15] = jlt,
    [0x16] = jgt,
    [0x17] = in,
    [0x18] = out,
    [0x19] = stw,
    [0x1a] = ldw,
    [0x1b] = _int,
};

void cpu_do_cycle(Cpu *cpu)
{
    uint8_t opcode = fetch(cpu);

    ops[opcode](cpu);
}
