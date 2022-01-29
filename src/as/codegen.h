#ifndef AS_CODEGEN_H
#define AS_CODEGEN_H
#include <ast.h>
#include <lib/map.h>
#include <lib/vec.h>
#include <stdint.h>

typedef uint8_t Byte;
typedef vec_t(Byte) Bytes;
typedef map_t(Byte) ByteMap;
typedef map_t(uint32_t) LabelMap;

typedef struct
{
    char *name;
    size_t byte_index;
} LabelReference;

typedef vec_t(LabelReference) LabelRefs;

Bytes codegen(Ast ast);
#endif