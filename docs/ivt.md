# IVT
IVT stands for interrupt vector table, it is a table used to describe how an interrupt should behave, giving it an handler, etc.

## Structure
The IVT looks like this:

```c
struct IvtEntry
{
    uint8_t used;
    uint8_t dpl;
    uint32_t address;
} IvtEntry;

struct Ivt
{
    IvtEntry entries[256];
};
```

## Cpu behavior
The CPU reloads the ivt when its register, #IVT, is modified.
Note that on each interrupt, the CPU pushes PC, therefore to return from an interrupt you need to pop PC

