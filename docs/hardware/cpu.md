# CPU
The CAAR CPU is designed to run LISP code efficiently, by having opcodes such as `CAR`, `CDR` and `CONS`

## Registers
There are 9 general registers and 6 special ones for a total of 15:

```
r0,r1,r2,r3,r4,r5,r6,r7,r8 -> General

PC -> Program Counter
SP -> Stack Pointer

IVT -> Points to the IVT
PT -> page table
PF -> Set to the address where a pagefault occured
```

## Flags
```
EQ -> Set after a CMP instruction if the operands are equal
LT -> Set after a CMP instruction if the first operand is smaller than the second one
PL -> clear if supervisor mode, set if user.
```

## Opcodes

The following is the list of cpu opcodes:

| Opcode    | Mnemonic | parameters       | Description                                                      |
| --------- | -------- | ---------------- | ---------------------------------------------------------------- |
| 00 (0x00) | CONS     | reg, reg/val     | Deprecated                                                       |
| 01 (0x01) | CAR      | reg, addr        | Get first element of cell, stored in first parameter             |
| 02 (0x02) | CDR      | reg, addr        | Get element of cell, stored in first parameter                   |
| 03 (0x03) | NOP      |                  | No OPeration                                                     |
| 04 (0x04) | LDR      | reg, addr        | Load addr into reg                                               |
| 05 (0x05) | STR      | reg/val, addr    | Store val at address                                             |
| 06 (0x06) | ADD      | reg, reg/val     | Add two numbers, result stored in the first parameter            |
| 07 (0x07) | SUB      | reg, reg/val     | Same as add but substracts                                       |
| 08 (0x08) | DIV      | reg, reg/val     | Same as add but divides                                          |
| 09 (0x09) | MUL      | reg, reg/val     | Same as add but multiplies                                       |
| 10 (0x0A) | MOD      | reg, reg/val     | Same as add but gets the remainder                               |
| 11 (0x0B) | NOT      | reg, reg/val     | Same as add but performs the "not" bitwise operator              |
| 12 (0x0C) | AND      | reg, reg/val     | Same as add but performs the "and" bitwise operator              |
| 13 (0x0D) | OR       | reg, reg/val     | Same as add but performs the "or" bitwise operator               |
| 14 (0x0E) | XOR      | reg, reg/val     | Same as add but performs the "xor" bitwise operator              |
| 15 (0x0F) | PUSH     | reg              | Pushes reg/val on the stack                                      |
| 16 (0x10) | POP      | reg              | Pops the stack into reg                                          |
| 17 (0x11) | JMP      | reg/addr         | Jumps to addr or addr stored in reg                              |
| 18 (0x12) | CMP      | reg/val, reg/val | Compares two values, if they are equal, set EQ flag to 1, else 0 |
| 19 (0x13) | JE       | reg/addr         | jmp if EQ is set                                                 |
| 20 (0x14) | JNE      | reg/addr         | jmp if EQ is clear                                               |
| 21 (0x15) | JLT      | reg/addr         | jmp if LT is set                                                 |
| 22 (0x16) | JGT      | reg/addr         | jmp if LT is clear and EQ is clear                               |
| 23 (0x17) | IN       | reg/val, reg     | Read from port to reg                                            |
| 24 (0x18) | OUT      | reg/val, reg/val | Write to port                                                    |
| 25 (0x19) | STW      | reg/val, addr    | Store word                                                       |
| 26 (0x1A) | LDW      | reg, addr        | Load word                                                        |
| 27 (0x1B) | INT      | reg/val          | Trigger interrupt                                                |
| 28 (0x1C) | CALL     | reg/addr         | Jump and push return address                                     |
| 29 (0x1D) | LSH      | reg, reg/val     | Left shift                                                       |
| 30 (0x1E) | RSH      | reg, reg/val     | Right shift                                                      |

## Instruction encoding
Each opcode parameter is followed by a special byte that tells the cpu what type of value it is.

Here is the byte's format:

`0b[param_type, 2 bits][register or number of bytes in imm, 6 bits]`
the param_type field is 2 bits and can represent the following values:

| Value (Binary) | Value type          |
| -------------- | ------------------- |
| 0b00           | register            |
| 0b10           | immediate value     |
| 0b11           | Address dereference |

the value field is 6 bits and can represent the following values:

| Value (Binary) | Value type |
| -------------- | ---------- |
| 0b000000       | 1 byte     |
| 0b000001       | 2 bytes    |
| 0b000010       | 4 bytes    |
| 0b000011       | 8 bytes    |

if the type is an immediate value, or if the type is register, it can represent the following values:

| Value (Binary) | Value type   |
| -------------- | ------------ |
| 0b000000       | register r0  |
| 0b000001       | register r1  |
| 0b000010       | register r2  |
| 0b000011       | register r3  |
| 0b000100       | register r4  |
| 0b000101       | register r5  |
| 0b000110       | register r6  |
| 0b000111       | register r7  |
| 0b001000       | register r8  |
| 0b001001       | register SP  |
| 0b001011       | register PC  |
| 0b001100       | register IVT |
| 0b001101       | register PT  |
| 0b001111       | register PF  |

## Paging

Addresses are 32 bit.
!!! Warning
	This section is **totally** work in progress. Paging is not implemented in the emulator yet.

## Interrupts

An interrupt is a type of signal emitted by hardware or software when an event needs immediate attention.

!!! Example
	Interrupts could be triggered when a key is pressed (hardware) or to perform a system call (software)

### The IVT
IVT stands for interrupt vector table, it is a table used to describe how an interrupt should behave, by giving it a certain privilege level or handler

The IVT, in C, would look like this:

```c
struct IvtEntry
{
    uint8_t used : 1; // Whether it is used or not
    uint8_t dpl : 1; // Minimum privilege level in which the software interrupt can be executed
    uint32_t address;
};

struct Ivt
{
    IvtEntry entries[256];
};
```

### Interrupt stackframe
The CPU automatically pushes `PC` when an interrupt occurs, therefore, to return from an interrupt you need to pop `PC`.

### Loading an IVT
To load an IVT, you can modify the `IVT` register.

!!! Note
	The `IVT` register is a *protected* register, meaning that it can only be modified in protection level 0

### Hardware interrupts
| Number   | Description                    |
| -------- | ------------------------------ |
| 0        | Invalid opcode (#UD)           |
| 1        | Page fault (#PF)               |
| 2        | General Protection fault (#GP) |
| 3        | Debug                          |
| 10 (0xA) | Timer interrupt                |
| 11 (0xB) | Keyboard                       |
