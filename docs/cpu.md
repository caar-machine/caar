# The CAAR CPU
The CAAR CPU is designed to run LISP code efficiently, by having opcodes such as `CAR`, `CDR` and `CONS`

## Addresses
Addresses are 32 bit

## Registers
There are 8 general registers and 2 special ones:

```
A,B,C,D,E,F,G,H -> general

PC -> program counter
SP -> stack pointer
```

## Flags
EQ -> set if cmp instruction is successful

## Opcodes

The following is the list of cpu opcodes:

| Opcode | Mnemonic | parameters       | Description                                                      |
| ------ | -------- | ---------------- | ---------------------------------------------------------------- |
| 00     | CONS     | reg, val         | CONStruct a pair of two cells, address stored in first parameter |
| 01     | CAR      | reg, addr        | Get first element of cell, stored in first parameter             |
| 02     | CDR      | reg, addr        | Get second element of cell, stored in first parameter            |
| 03     | NOP      |                  | No OPeration                                                     |
| 04     | LDR      | reg, addr        | Load addr into reg                                               |
| 05     | STR      | reg/val, addr    | Store val at address                                             |
| 06     | ADD      | reg, reg/val     | Add two numbers, result stored in the first parameter            |
| 07     | SUB      | reg, reg/val     | Same as add but substracts                                       |
| 08     | DIV      | reg, reg/val     | Same as add but divides                                          |
| 09     | MUL      | reg, reg/val     | Same as add but multiplies                                       |
| 10     | MOD      | reg, reg/val     | Same as add but gets the remainder                               |
| 11     | NOT      | reg, reg/val     | Same as add but performs the "not" bitwise operator              |
| 12     | AND      | reg, reg/val     | Same as add but performs the "and" bitwise operator              |
| 13     | OR       | reg, reg/val     | Same as add but performs the "or" bitwise operator               |
| 14     | XOR      | reg, reg/val     | Same as add but performs the "xor" bitwise operator              |
| 15     | PUSH     | reg              | Pushes reg/val on the stack                                      |
| 16     | POP      | reg              | Pops the stack into reg                                          |
| 17     | JMP      | reg/addr         | Jumps to addr or addr stored in reg                              |
| 18     | CMP      | reg/val, reg/val | Compares two values, if they are equal, set EQ flag to 1, else 0 |
| 19     | JE       | reg/addr         | jmp if EQ is 1                                                   |
| 20     | JNE      | reg/addr         | jmp if EQ is 0                                                   |
| 21     | IN       | reg/val, reg     | read from port to reg                                            |
| 22     | OUT      | reg/val, reg/val | write to port                                                    |

## Instruction encoding
Each opcode parameter is followed by a special byte that tells the cpu what type of value it is.
therefore, `(add A 2)` is `0x6 0x1b 0x1a 0x27 2` (add + A + immediate value + byte + 2)

| Value  | Type                   |
| ------ | ---------------------- |
| 26     | immediate value        |
| 27..36 | registers from A to SP |
| 37     | address dereference    |
| 38     | one byte               |
| 39     | two bytes              |
| 40     | three bytes            |
| 41     | four bytes             |

`(add [1] 2)` is `0x6 0x26 1 0x1a 2`