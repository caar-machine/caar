#include "dev/io.h"
#include "ram.h"
#include <cpu.h>
#include <dev/bus.h>
#include <ivt.h>
#include <lib/log.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define CPU_OP(OP)                                     \
    uint32_t *ptr = decode(fetch(cpu), true, cpu).ptr; \
    uint32_t rhs = decode(fetch(cpu), false, cpu).val; \
    *ptr = *ptr OP rhs;

#define CPU_SOP(OP)                                    \
    uint32_t *ptr = decode(fetch(cpu), true, cpu).ptr; \
    uint32_t rhs = decode(fetch(cpu), false, cpu).val; \
    *ptr = OP rhs;

#define CPU_CELL_OP(TYPE)                              \
    uint32_t lhs = decode(fetch(cpu), true, cpu).val;  \
    uint32_t rhs = decode(fetch(cpu), false, cpu).val; \
    ram_write(lhs + offsetof(Cons, TYPE), MEM_4_BYTES, rhs, cpu->ram);

#define CPU_GET_LHS_AND_RHS()                          \
    uint32_t lhs = decode(fetch(cpu), false, cpu).val; \
    uint32_t rhs = decode(fetch(cpu), false, cpu).val;

#define CPU_GET_LHS_AND_SET_RHS()                      \
    uint32_t *lhs = decode(fetch(cpu), true, cpu).ptr; \
    uint32_t rhs = decode(fetch(cpu), false, cpu).val;

#define CPU_GET_VAL() \
    uint32_t val = decode(fetch(cpu), false, cpu).val;

#define CPU_SET_VAL() \
    uint32_t *val = decode(fetch(cpu), false, cpu).ptr;

uint8_t fetch(Cpu *cpu)
{
    uint32_t value = 0;

    ram_read(cpu->regs[CPU_PC]++, MEM_BYTE, &value, cpu->ram);

    return value;
}

void push(uint32_t what, Cpu *cpu)
{
    cpu->regs[CPU_SP] -= 4;

    if (cpu->regs[CPU_SP] == STACK_SIZE)
    {
        error("stack overflow");
    }

    if (what > 255)
    {
        uint8_t *a = (uint8_t *)&what;

        ram_write(cpu->regs[CPU_SP] - 1, MEM_BYTE, a[0], cpu->ram);
        ram_write(cpu->regs[CPU_SP] - 2, MEM_BYTE, a[1], cpu->ram);
        ram_write(cpu->regs[CPU_SP] - 3, MEM_BYTE, a[2], cpu->ram);
        ram_write(cpu->regs[CPU_SP] - 4, MEM_BYTE, a[3], cpu->ram);
    }

    else
        ram_write(cpu->regs[CPU_SP] - 4, MEM_BYTE, what, cpu->ram);
}

uint32_t pop(Cpu *cpu)
{

    if (cpu->regs[CPU_SP] > MEMORY_SIZE)
    {
        error("pop() out of bounds");
        return 0;
    }

    uint32_t ret = 0;
    ram_read(cpu->regs[CPU_SP] - 4, MEM_4_BYTES, &ret, cpu->ram);

    cpu->regs[CPU_SP] += 4;

    return ret;
}

void cpu_init(Ram *ram, Bus *bus, Cpu *cpu, size_t rom_size)
{
    cpu->ram = ram;
    cpu->bus = bus;
    cpu->regs[CPU_SP] = MEMORY_SIZE;
    cpu->regs[CPU_PC] = 0x1000;
    cpu->rom_size = rom_size;
    cpu->flags.EQ = 0;
    cpu->flags.LT = 0;
    cpu->flags.PL = 0;
}

InstructionDecoding decode(uint8_t byte, bool set, Cpu *cpu)
{
    InstructionEncoding encoding = *(InstructionEncoding *)&byte;
    InstructionDecoding ret = {0};

    info("val: %x, param_type: %x", (encoding.value) | 0b0000, encoding.param_type);
    cpu->regs[CPU_PC]++;

    if (encoding.param_type == 0)
    {
        ret.type = set ? DECODE_IMM : DECODE_REG;
        if (set)
            ret.ptr = &cpu->regs[encoding.value];
        else
            ret.val = cpu->regs[encoding.value];

        cpu->regs[CPU_PC]++;
    }

    else if (encoding.param_type == 2)
    {
        ret.type = DECODE_IMM;

        ram_read(cpu->regs[CPU_PC], encoding.value, &ret.val, cpu->ram);

        info("Adding %x to pc", (encoding.value == 2 ? 4 : encoding.value + 1));

        cpu->regs[CPU_PC] += (encoding.value == 2 ? 4 : encoding.value + 1);
    }

    return ret;
}

void cons(Cpu *cpu)
{
    CPU_GET_LHS_AND_SET_RHS();

    Cons *buffer = ram_allocate(cpu->ram);

    buffer->car = *lhs;
    buffer->cdr = rhs;

    *lhs = (uint64_t)buffer - (uint64_t)cpu->ram->buffer;
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

    CPU_GET_LHS_AND_SET_RHS();

    uint32_t value = 0;

    bus_read(rhs, MEM_BYTE, &value, cpu->ram, cpu->bus);

    *lhs = value;
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
    CPU_GET_VAL();

    push(val, cpu);
}

void _pop(Cpu *cpu)
{
    CPU_SET_VAL();

    *val = pop(cpu);
}

void jmp(Cpu *cpu)
{

    CPU_GET_VAL();

    info("%x", val);
    cpu->regs[CPU_PC] = val;
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
}

void je(Cpu *cpu)
{
    CPU_GET_VAL();

    if (cpu->flags.EQ == 1)
        cpu->regs[CPU_PC] = val;
}

void jne(Cpu *cpu)
{

    CPU_GET_VAL();

    if (cpu->flags.EQ == 0)
        cpu->regs[CPU_PC] = val;
}

void jlt(Cpu *cpu)
{
    CPU_GET_VAL();

    if (cpu->flags.LT == 1)
        cpu->regs[CPU_PC] = val;
}

void jgt(Cpu *cpu)
{
    CPU_GET_VAL();

    if (cpu->flags.LT == 0 && cpu->flags.EQ == 0)
        cpu->regs[CPU_PC] = val;
}

void in(Cpu *cpu)
{
    CPU_GET_LHS_AND_SET_RHS();

    *lhs = io_read(rhs);
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
    CPU_GET_LHS_AND_SET_RHS();

    (void)lhs;

    uint32_t value = 0;

    bus_read(rhs, MEM_4_BYTES, &value, cpu->ram, cpu->bus);

    *lhs = value;
}

void _int(Cpu *cpu)
{
    CPU_GET_VAL();
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

    if (opcode > 0x1b)
    {
        ivt_trigger_interrupt(0, false, cpu);
        error("Invalid opcode: %x", opcode);
    }
    else
    {
        ops[opcode](cpu);
    }
}
