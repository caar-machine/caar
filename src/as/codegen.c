#include "ast.h"
#include <codegen.h>
#include <ctype.h>
#include <lib/log.h>
#include <lib/map.h>
#include <stdbool.h>

static uint32_t current_addr = 0;

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

typedef map_t(Byte) ByteMap;
typedef map_t(uint32_t) LabelMap;

// Expressions are registers or immediate values.
// TODO: add support for dereferences.
void codegen_expr(AstValue value, Bytes *bytes, LabelMap *labels, bool defining)
{
    switch (value.type)
    {

    case AST_VAL_INT:
    {
        uint32_t val = value.int_;

        vec_push(bytes, 0x1A);
        vec_push(bytes, (val > 0xFFFFFF) ? 0x29 : ((val > 0x00FFFF) ? 0x28 : ((val > 0x0000FF) ? 0x27 : 0x26)));

        Byte *_bytes = (Byte *)&val;

        // Get size of value
        if (val > 0xFFFFFF)
        {
            // if four bytes, push 4 bytes
            vec_push(bytes, _bytes[0]);
            vec_push(bytes, _bytes[1]);
            vec_push(bytes, _bytes[2]);
            vec_push(bytes, _bytes[3]);
        }
        else if (val > 0xFFFF)
        {
            // three bytes
            vec_push(bytes, _bytes[0]);
            vec_push(bytes, _bytes[1]);
            vec_push(bytes, _bytes[2]);
        }
        else if (val > 0xFF)
        {
            // 2 bytes
            vec_push(bytes, _bytes[1]);
            vec_push(bytes, _bytes[0]);
        }
        else
        {
            vec_push(bytes, _bytes[0]);
        }
        break;
    }

    case AST_VAL_SYMBOL:
    {
        uint32_t *addr = map_get(labels, value.symbol_);

        if (!addr)
        {
            error("Unknown label: %s\n", value.symbol_);
            exit(1);
        }

        vec_push(bytes, 0x1A);
        vec_push(bytes, 0x29);

        Byte *_bytes = (Byte *)addr;
        vec_push(bytes, _bytes[3]);
        vec_push(bytes, _bytes[2]);
        vec_push(bytes, _bytes[1]);
        vec_push(bytes, _bytes[0]);

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
            vec_push(bytes, value.str_[i]);
        }

        break;
    }

    case AST_VAL_REG:
    {
        if (value.reg_ > REG_SP)
        {
            error("Invalid register.");
            error("This is most likely an internal compiler bug or you messed up.");
        }

        vec_push(bytes, 0x1B + value.reg_);

        break;
    }
    }
}

void codegen_call(AstCall call, LabelMap *labels, ByteMap opcodes, Bytes *bytes)
{
    Byte *opcode = map_get(&opcodes, call.name);

    if (!opcode)
    {
        if (!strcmp(call.name, "org"))
        {
            current_addr = call.params.data[0].int_;
            return;
        }

        else if (!strcmp(call.name, "label"))
        {
            map_set(labels, call.params.data->str_, current_addr);
            return;
        }

        else
        {
            error("No such instruction: %s", call.name);
        }
    }

    vec_push(bytes, *opcode);

    for (int i = 0; i < call.params.length; i++)
    {
        codegen_expr(call.params.data[i], bytes, labels, false);
    }
}

Bytes codegen_impl(Ast ast, ByteMap opcodes, LabelMap labels)
{
    Bytes ret;
    vec_init(&ret);

    size_t prev_len = 0;

    for (int i = 0; i < ast.length; i++)
    {
        switch (ast.data[i].type)
        {

        case AST_CALL:
        {
            codegen_call(ast.data[i].call, &labels, opcodes, &ret);
            break;
        }

        case AST_VALUE:
        {
            break;
        }
        }

        current_addr += (ret.length - prev_len);
        prev_len = ret.length;
    }

    return ret;
}

Bytes codegen(Ast ast)
{
    LabelMap labels;
    map_init(&labels);

    ByteMap opcodes;
    map_init(&opcodes);

    map_set(&opcodes, "cons", 0x00);
    map_set(&opcodes, "car", 0x01);
    map_set(&opcodes, "cdr", 0x02);
    map_set(&opcodes, "nop", 0x03);
    map_set(&opcodes, "ldr", 0x04);
    map_set(&opcodes, "str", 0x05);
    map_set(&opcodes, "add", 0x06);
    map_set(&opcodes, "sub", 0x07);
    map_set(&opcodes, "div", 0x08);
    map_set(&opcodes, "mul", 0x09);
    map_set(&opcodes, "mod", 0x0A);
    map_set(&opcodes, "not", 0x0B);
    map_set(&opcodes, "and", 0x0C);
    map_set(&opcodes, "or", 0x0D);
    map_set(&opcodes, "xor", 0x0E);
    map_set(&opcodes, "push", 0x0F);
    map_set(&opcodes, "pop", 0x10);
    map_set(&opcodes, "jmp", 0x11);
    map_set(&opcodes, "cmp", 0x12);
    map_set(&opcodes, "je", 0x13);
    map_set(&opcodes, "jne", 0x14);
    map_set(&opcodes, "jlt", 0x15);
    map_set(&opcodes, "jgt", 0x16);
    map_set(&opcodes, "in", 0x17);
    map_set(&opcodes, "out", 0x18);

    return codegen_impl(ast, opcodes, labels);
}