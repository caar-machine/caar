#include <codegen.h>
#include <lib/map.h>
#include <stdbool.h>

#define LITTLE_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)
#define CHECK_PARAM(params, idx, t) (idx < params.length && params.data[idx].type == t)

char *str_to_lower(char *str)
{
    char *ret = str;
    while (*str)
    {
        if (*str >= 'A' && *str <= 'Z')
            *str += 'a' - 'A';
        str++;
    }

    return ret;
}

typedef map_t(Byte) ByteMap;
typedef map_t(uint32_t) LabelMap;

Bytes _codegen(Ast ast, int *index, ByteMap opcodes, LabelMap *labels, bool toplevel, bool db)
{
    Bytes ret = {};
    vec_init(&ret);

    for (; *index < ast.length; *index += 1)
    {
        switch (ast.data[*index].type)
        {
        case AST_CALL:
		{
            if (!toplevel)
            {
                fprintf(stderr, "Function calls need to be at top level\n");
                exit(1);
            }

            if (strcmp(ast.data[*index].call.name, "label") == 0)
            {
                if (!CHECK_PARAM(ast.data[*index].call.params, 0, AST_VAL_SYMBOL))
                {
                    fprintf(stderr, "label requires parameters (symbol)\n");
                    exit(1);
                }

                map_set(labels, ast.data[*index].call.params.data[0].symbol_, ret.length + 0x1000);
            }
			if (strcmp(ast.data[*index].call.name, "db") == 0)
            {
                int idx;
                Bytes bytes = _codegen(astvalues_to_ast(ast.data[*index].call.params), &idx, opcodes, labels, false, true);
				Byte byte;
                vec_foreach(&bytes, byte, idx)
                {
                    vec_push(&ret, byte);
                }
            }
            else if (map_get(&opcodes, str_to_lower(ast.data[*index].call.name)))
            {
                Byte opcode = *map_get(&opcodes, str_to_lower(ast.data[*index].call.name));
                AstValues params = ast.data[*index].call.params;

                vec_push(&ret, opcode);
                switch (opcode)
                {
                case 0x00: // CONS
                {
                    if (!CHECK_PARAM(params, 0, AST_VAL_REG))
                    {
                        fprintf(stderr, "%d %d\n", params.data[0].type, params.data[1].type);
                        fprintf(stderr, "cons requires parameters (reg, val | reg)\n");
                        exit(1);
                    }

                    if (params.length != 2)
                    {
                        fprintf(stderr, "cons requires parameters (reg, val | reg)\n");
                        exit(1);
                    }

                    int idx;
                    Byte byte;
                    Ast params_ast = astvalues_to_ast(params);
                    Bytes params_gen = _codegen(params_ast, index, opcodes, labels, false, false);
                    vec_foreach(&params_gen, byte, idx)
                    {
                        vec_push(&ret, byte);
                    }

                    vec_deinit(&params_gen);
                    vec_deinit(&params_ast);

                    break;
                }

				case 0x01: // CAR
                {
                    if (!CHECK_PARAM(params, 0, AST_VAL_REG))
                    {
                        fprintf(stderr, "%d %d\n", params.data[0].type, params.data[1].type);
                        fprintf(stderr, "cons requires parameters (reg, val | reg)\n");
                        exit(1);
                    }

                    if (params.length != 2)
                    {
                        fprintf(stderr, "cons requires parameters (reg, val | reg)\n");
                        exit(1);
                    }

                    int idx;
                    Byte byte;
                    Ast params_ast = astvalues_to_ast(params);
                    Bytes params_gen = _codegen(params_ast, index, opcodes, labels, false, false);
                    vec_foreach(&params_gen, byte, idx)
                    {
                        vec_push(&ret, byte);
                    }

                    vec_deinit(&params_gen);
                    vec_deinit(&params_ast);

                    break;
                }

				case 0x02: // CDR
                {
                    if (!CHECK_PARAM(params, 0, AST_VAL_REG))
                    {
                        fprintf(stderr, "%d %d\n", params.data[0].type, params.data[1].type);
                        fprintf(stderr, "cons requires parameters (reg, val | reg)\n");
                        exit(1);
                    }

                    if (params.length != 2)
                    {
                        fprintf(stderr, "cons requires parameters (reg, val | reg)\n");
                        exit(1);
                    }

                    int idx;
                    Byte byte;
                    Ast params_ast = astvalues_to_ast(params);
                    Bytes params_gen = _codegen(params_ast, index, opcodes, labels, false, false);
                    vec_foreach(&params_gen, byte, idx)
                    {
                        vec_push(&ret, byte);
                    }

                    vec_deinit(&params_gen);
                    vec_deinit(&params_ast);

                    break;
                }

				case 0x03: // NOP
					break;

                case 0x04: // LDR
                {
                    if (!CHECK_PARAM(params, 0, AST_VAL_REG))
                    {
                        fprintf(stderr, "ldr requires parameters (reg, val | reg)\n");
                        exit(1);
                    }

                    if (2 != params.length)
                    {
                        fprintf(stderr, "ldr requires parameters (reg, val | reg)\n");
                        exit(1);
                    }

                    int idx;
                    Byte byte;
                    Ast params_ast = astvalues_to_ast(params);
                    Bytes params_gen = _codegen(params_ast, index, opcodes, labels, false, false);
                    vec_foreach(&params_gen, byte, idx)
                    {
                        vec_push(&ret, byte);
                    }

                    vec_deinit(&params_gen);
                    vec_deinit(&params_ast);

                    break;
                }

                case 0x05: // STR
                {
                    if (2 != params.length)
                    {
                        fprintf(stderr, "str requires parameters (val | reg, val | reg)\n");
                        exit(1);
                    }

                    int idx;
                    Byte byte;
                    Ast params_ast = astvalues_to_ast(params);
                    Bytes params_gen = _codegen(params_ast, index, opcodes, labels, false, false);
                    vec_foreach(&params_gen, byte, idx)
                    {
                        vec_push(&ret, byte);
                    }

                    vec_deinit(&params_gen);
                    vec_deinit(&params_ast);

                    break;
                }

                default:
                    fprintf(stderr, "Unimplemented opcode %s\n", ast.data[*index].call.name);
                    exit(1);
                };
            }
            else
            {
                printf("Unknown function: %s\n", ast.data[*index].call.name);
                exit(1);
            }

            break;
		}

        case AST_VALUE:
            if (toplevel)
            {
                fprintf(stderr, "Values cannot be at top level\n");
                exit(1);
            }

            switch (ast.data[*index].value.type)
            {
            case AST_VAL_INT:
            {
                uint32_t val = ast.data[*index].value.int_;

                vec_push(&ret, 0x1A);
                vec_push(
                    &ret,
                    (val > 0xFFFFFF) ? 0x29 : ((val > 0x00FFFF) ? 0x28 : ((val > 0x0000FF) ? 0x27 : 0x26)));

                if (LITTLE_ENDIAN)
                {
                    Byte *bytes = (Byte *)&val;
                    if (val > 0xFFFFFF)
                    {
                        vec_push(&ret, bytes[0]);
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[2]);
                        vec_push(&ret, bytes[3]);
                    }
                    else if (val > 0xFFFF)
                    {
                        vec_push(&ret, bytes[0]);
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[2]);
                    }
                    else if (val > 0xFF)
                    {
                        vec_push(&ret, bytes[0]);
                        vec_push(&ret, bytes[1]);
                    }
                    else
                    {
                        vec_push(&ret, bytes[0]);
                    }
                }
                else
                {
                    Byte *bytes = (Byte *)&val;
                    if (val > 0xFFFFFF)
                    {
                        vec_push(&ret, bytes[3]);
                        vec_push(&ret, bytes[2]);
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[0]);
                    }
                    else if (val > 0xFFFF)
                    {
                        vec_push(&ret, bytes[2]);
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[0]);
                    }
                    else if (val > 0xFF)
                    {
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[0]);
                    }
                    else
                    {
                        vec_push(&ret, bytes[0]);
                    }
                }

                break;
            }

            case AST_VAL_SYMBOL:
            {
                if (!map_get(labels, ast.data[*index].value.symbol_))
                {
                    fprintf(stderr, "Unknown label: %s\n", ast.data[*index].value.symbol_);
                    exit(1);
                }

                uint32_t val = *map_get(labels, ast.data[*index].value.symbol_);

                vec_push(&ret, 0x1A);
                vec_push(
                    &ret,
                    (val > 0xFFFFFF) ? 0x29 : ((val > 0x00FFFF) ? 0x28 : ((val > 0x0000FF) ? 0x27 : 0x26)));

                if (LITTLE_ENDIAN)
                {
                    Byte *bytes = (Byte *)&val;
                    if (val > 0xFFFFFF)
                    {
                        vec_push(&ret, bytes[0]);
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[2]);
                        vec_push(&ret, bytes[3]);
                    }
                    else if (val > 0xFFFF)
                    {
                        vec_push(&ret, bytes[0]);
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[2]);
                    }
                    else if (val > 0xFF)
                    {
                        vec_push(&ret, bytes[0]);
                        vec_push(&ret, bytes[1]);
                    }
                    else
                    {
                        vec_push(&ret, bytes[0]);
                    }
                }
                else
                {
                    Byte *bytes = (Byte *)&val;
                    if (val > 0xFFFFFF)
                    {
                        vec_push(&ret, bytes[3]);
                        vec_push(&ret, bytes[2]);
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[0]);
                    }
                    else if (val > 0xFFFF)
                    {
                        vec_push(&ret, bytes[2]);
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[0]);
                    }
                    else if (val > 0xFF)
                    {
                        vec_push(&ret, bytes[1]);
                        vec_push(&ret, bytes[0]);
                    }
                    else
                    {
                        vec_push(&ret, bytes[0]);
                    }
                }

                break;
            }

			case AST_VAL_STR:
			{
				if (!db)
				{
					fprintf(stderr, "ERROR: Strings can only be stored\n");
					exit(1);
				}
				
				for (size_t i = 0; i < strlen(ast.data[*index].value.str_); i++)
				{
					vec_push(&ret, ast.data[*index].value.str_[i]);
				}
				
				break;
			}

            case AST_VAL_REG:
            {
                if (ast.data[*index].value.reg_ > REG_SP)
                {
                    fprintf(stderr, "ERROR: Unhandled error\n");
                    exit(1);
                }

                vec_push(&ret, 0x1B + ast.data[*index].value.reg_);

                break;
            }

            default:
                fprintf(stderr, "ERROR: Unknown value type\n");
                exit(1);
            }
            break;
        }
    }

    return ret;
}

Bytes codegen(Ast ast)
{
    int i = 0;
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

    return _codegen(ast, &i, opcodes, &labels, true, false);
}