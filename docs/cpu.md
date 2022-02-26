# The CAAR CPU
The CAAR CPU is designed to run LISP code efficiently, by having opcodes such as `CAR`, `CDR` and `CONS`

## Addresses
Addresses are 32 bit

## Registers
There are 8 general registers and 6 special ones:

```
r0,r1,r2,r3,r4,r5,r6,r7 -> General

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
| 00 (0x00) | CONS     | reg, reg/val     | CONStruct a pair of two cells, address stored in first parameter |
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
| 0b001000       | register SP  |
| 0b001001       | register PC  |
| 0b001011       | register IVT |
| 0b001011       | register PT  |
| 0b001100       | register PF  |
