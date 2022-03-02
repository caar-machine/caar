#include "ast.h"
#include <codegen.h>
#include <ctype.h>
#include <lib/log.h>
#include <lib/map.h>
#include <stdbool.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)       \
    (byte & 0x80 ? '1' : '0'),     \
        (byte & 0x40 ? '1' : '0'), \
        (byte & 0x20 ? '1' : '0'), \
        (byte & 0x10 ? '1' : '0'), \
        (byte & 0x08 ? '1' : '0'), \
        (byte & 0x04 ? '1' : '0'), \
        (byte & 0x02 ? '1' : '0'), \
        (byte & 0x01 ? '1' : '0')

#define OPCODES_MAP(what)        \
    map_set(what, "cons", 0x00); \
    map_set(what, "car", 0x01);  \
    map_set(what, "cdr", 0x02);  \
    map_set(what, "nop", 0x03);  \
    map_set(what, "ldr", 0x04);  \
    map_set(what, "str", 0x05);  \
    map_set(what, "add", 0x06);  \
    map_set(what, "sub", 0x07);  \
    map_set(what, "div", 0x08);  \
    map_set(what, "mul", 0x09);  \
    map_set(what, "mod", 0x0A);  \
    map_set(what, "not", 0x0B);  \
    map_set(what, "and", 0x0C);  \
    map_set(what, "or", 0x0D);   \
    map_set(what, "xor", 0x0E);  \
    map_set(what, "push", 0x0F); \
    map_set(what, "pop", 0x10);  \
    map_set(what, "jmp", 0x11);  \
    map_set(what, "cmp", 0x12);  \
    map_set(what, "je", 0x13);   \
    map_set(what, "jne", 0x14);  \
    map_set(what, "jlt", 0x15);  \
    map_set(what, "jgt", 0x16);  \
    map_set(what, "in", 0x17);   \
    map_set(what, "out", 0x18);  \
    map_set(what, "stw", 0x19);  \
    map_set(what, "ldw", 0x1A);  \
    map_set(what, "int", 0x1B);  \
    map_set(what, "call", 0x1C); \
    map_set(what, "mov", 0x1D);

char *str_to_lower(char *str)
{
    char *ret = str;

    while (*str)
    {
        *str = tolower(*str);
        str++;
    }

    return ret;
}

// Expressions are registers or immediate values.
// TODO: add support for dereferences.
void codegen_expr(AstValue value, Assembler *as, bool defining, bool is_macro, bool word)
{
    switch (value.type)
    {

    case AST_VAL_INT:
    {
        uint32_t val = value.int_;

        if (!defining)
        {
            uint8_t byte = (val > 0xFFFF) ? 0b10000010 : ((val > 0x0000FF) ? 0b10000001 : 0b10000000);

            vec_push(&as->bytes, byte);
        }

        Byte *_bytes = (Byte *)&val;

        if (!word)
        {
            // Get size of value
            if (val > 0xFFFF)
            {

                // if four bytes, push 4 bytes
                vec_push(&as->bytes, _bytes[0]);
                vec_push(&as->bytes, _bytes[1]);
                vec_push(&as->bytes, _bytes[2]);
                vec_push(&as->bytes, val > 0xFFFFFF ? _bytes[3] : 0);
            }
            else if (val > 0xFF)
            {

                // 2 bytes
                vec_push(&as->bytes, _bytes[0]);
                vec_push(&as->bytes, _bytes[1]);
            }
            else
            {
                vec_push(&as->bytes, _bytes[0]);
            }
        }

        else
        {
            vec_push(&as->bytes, _bytes[0]);
            vec_push(&as->bytes, _bytes[1]);
            vec_push(&as->bytes, _bytes[2]);
            vec_push(&as->bytes, _bytes[3]);
        }

        break;
    }

    case AST_VAL_SYMBOL:
    {
        if (!defining)
        {
            vec_push(&as->bytes, 0b10000010);
        }

        char *what = value.symbol_;

        if (is_macro)
        {
            AstValue *val = map_get(&as->current_macro->symbols, value.symbol_);

            if (val)
            {
                what = val->symbol_;
            }
        }

        LabelReference new_ref = {.name = strdup(what), .byte_index = is_macro ? as->original_bytes.length + as->bytes.length : as->bytes.length};

        vec_push(&as->bytes, 0);
        vec_push(&as->bytes, 0);
        vec_push(&as->bytes, 0);
        vec_push(&as->bytes, 0);

        vec_push(&as->refs, new_ref);

        break;
    }

    case AST_VAL_STR:
    {

        if (strlen(value.str_) > 4 && !defining)
        {
            error("String constant too big (4+ bytes)");
        }

        for (size_t i = 0; i < strlen(value.str_); i++)
        {
            vec_push(&as->bytes, value.str_[i]);
        }

        break;
    }

    case AST_VAL_REG:
    {
        uint8_t byte = 0b00;
        uint8_t reg = value.reg_;
        uint8_t result = (byte << 2) | reg;

        vec_push(&as->bytes, result);

        break;
    }
    }
}

void codegen_impl(Assembler *as, bool is_macro);

void codegen_macro(Macro *macro, Assembler *as)
{
    as->macro_ast = macro->ast_macro->body;

    codegen_impl(as, true);

    for (int i = 0; i < macro->bytes.length; i++)
    {
        vec_push(&as->bytes, macro->bytes.data[i]);
    }
}

void codegen_call(AstCall call, Assembler *as, bool is_macro)
{
    if (is_macro)
    {
        AstValue *val = map_get(&as->current_macro->symbols, call.name);

        if (val)
            call.name = val->symbol_;
    }

    Byte *opcode = map_get(&as->opcodes, call.name);

    if (!opcode)
    {
        Macro *macro = map_get(&as->macros, call.name);

        if (macro)
        {
            as->current_macro = macro;

            for (int i = 0; i < macro->ast_macro->params.length; i++)
            {
                map_set(&macro->symbols, macro->ast_macro->params.data[0], call.params.data[0]);
            }

            codegen_macro(macro, as);

            return;
        }

        if (!strcmp(call.name, "org"))
        {
            as->current_addr = call.params.data[0].int_;
            return;
        }

        else if (!strcmp(call.name, "label"))
        {
            map_set(&as->labels, call.params.data->str_, as->current_addr);
            return;
        }

        else if (!strcmp(call.name, "db"))
        {
            for (int i = 0; i < call.params.length; i++)
            {
                codegen_expr(call.params.data[i], as, true, is_macro, false);
            }

            return;
        }
        else if (!strcmp(call.name, "dw"))
        {
            for (int i = 0; i < call.params.length; i++)
            {
                codegen_expr(call.params.data[i], as, true, is_macro, true);
            }

            return;
        }
        else if (!strcmp(call.name, "include"))
        {
            return;
        }

        else
        {
            error("No such instruction: %s", call.name);
        }
    }

    vec_push(&as->bytes, *opcode);

    for (int i = 0; i < call.params.length; i++)
    {
        codegen_expr(call.params.data[i], as, false, is_macro, false);
    }
}

void codegen_impl(Assembler *as, bool is_macro)
{
    Ast ast = is_macro ? as->macro_ast : as->ast;

    for (int i = 0; i < ast.length; i++)
    {
        switch (ast.data[i].type)
        {

        case AST_CALL:
        {
            codegen_call(ast.data[i].call, as, is_macro);
            break;
        }

        case AST_VALUE:
        {
            break;
        }

        case AST_MACRO:
        {
            Macro macro = {0};
            macro.ast_macro = &ast.data[i].macro;

            vec_init(&macro.bytes);

            map_set(&as->macros, ast.data[i].macro.name, macro);

            break;
        }

        default:
            break;
        }

        as->current_addr += (as->bytes.length - as->prev_len);
        as->prev_len = as->bytes.length;
    }
}

Bytes codegen(Ast ast)
{
    Assembler as = {0};

    as.ast = ast;

    map_init(&as.labels);
    map_init(&as.opcodes);
    vec_init(&as.refs);
    vec_init(&as.bytes);

    OPCODES_MAP(&as.opcodes);

    codegen_impl(&as, false);

    for (int i = 0; i < as.refs.length; i++)
    {
        LabelReference ref = as.refs.data[i];

        uint32_t *addr = map_get(&as.labels, ref.name);

        if (!addr)
        {
            error("Undefined symbol '%s' ", ref.name);
            exit(-1);
        }

        Byte *byte = (Byte *)addr;

        as.bytes.data[ref.byte_index] = byte[0];
        as.bytes.data[ref.byte_index + 1] = byte[1];
        as.bytes.data[ref.byte_index + 2] = byte[2];
        as.bytes.data[ref.byte_index + 3] = byte[3];
    }

    return as.bytes;
}
