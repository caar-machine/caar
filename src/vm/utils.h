#ifndef CAAR_UTILS_H
#define CAAR_UTILS_H
#include <cpu.h>

uint8_t fetch(Cpu *cpu);
void push(uint32_t what, Cpu *cpu);

uint32_t pop(Cpu *cpu);

uint32_t get_val_from_special_byte(uint32_t *size, Cpu *cpu);

void set_from_special_byte(uint32_t val, Cpu *cpu);

void pop_from_special_byte(Cpu *cpu);

#endif