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

typedef map_t(AstValue) MacroSymbols;

typedef struct
{
    AstMacro *ast_macro;
    Bytes bytes;
    MacroSymbols symbols;
} Macro;

typedef map_t(Macro) MacroMap;
typedef struct
{
    Bytes bytes;
    LabelMap labels;
    ByteMap opcodes;
    MacroMap macros;
    LabelRefs refs;
    Ast ast;
    uint32_t current_addr, prev_len;
    Ast macro_ast;
    Bytes original_bytes;
    Macro *current_macro;
} Assembler;

typedef struct __attribute__((packed))
{
    uint8_t param_type : 2;
    uint8_t value : 6;
} InstructionEncoding;

#endif
