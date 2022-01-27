# The CAAR CPU
The CAAR CPU is designed to run LISP code efficiently, by having opcodes such as `CAR`, `CDR` and `CONS`

## Addresses
Addresses are 32 bit

## Registers
There are 8 general registers and 2 special ones:

```
A,B,C,D,E,F,G,H -> General

PC -> Program Counter
SP -> Stack Pointer
```

## Flags
EQ -> Set after a CMP instruction if the operands are equal
LT -> Set after a CMP instruction if the first operator is less than the second one

## Opcodes

The following is the list of cpu opcodes:

| Opcode    | Mnemonic | parameters       | Description                                                      |
| --------- | -------- | ---------------- | ---------------------------------------------------------------- |
| 00 (0x00) | CONS     | reg, reg/val     | CONStruct a pair of two cells, address stored in first parameter |
| 01 (0x01) | CAR      | reg, addr        | Get first element of cell, stored in first parameter             |
| 02 (0x02) | CDR      | reg, addr        | Get second element of cell, stored in first parameter            |
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

## Instruction encoding
Each opcode parameter is followed by a special byte that tells the cpu what type of value it is.
therefore, `(add A 2)` is `0x6 0x1b 0x1a 0x27 2` (add + A + immediate value + byte + 2)

| Value               | Type                   |
| ------------------- | ---------------------- |
| 26     (0x1A)       | Immediate value        |
| 27..36 (0x1B..0x24) | Registers from A to SP |
| 37     (0x25)       | Address dereference    |
| 38     (0x26)       | One byte               |
| 39     (0x27)       | Two bytes              |
| 40     (0x28)       | Three bytes            |
| 41     (0x29)       | Four bytes             |

`(add [1] 2)` is `0x6 0x26 1 0x1a 2`