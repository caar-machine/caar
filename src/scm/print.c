#include <print.h>

void scm_print_list(Ast list, FILE *output)
{
    fprintf(output, "[");

    for (int i = 0; i < list.length; i++)
    {
        scm_print(list.data[i], output);
        fprintf(output, "%c", i == list.length - 1 ? '\0' : ',');
    }

    fprintf(output, "]");
}

void scm_print_value(AstValue value, FILE *output)
{
    if (value.quoted)
    {
        fprintf(output, "'");
    }

    switch (value.type)
    {

    case AST_VAL_LIST:
    {
        scm_print_list(value.list, output);
        break;
    }

    case AST_VAL_INT:
    {
        fprintf(output, "%d", value.int_);
        break;
    }

    case AST_VAL_SYMBOL:
    {
        fprintf(output, "%s", value.symbol);
        break;
    }

    case AST_VAL_STR:
    {
        fprintf(output, "\"%s\"", value.str);
        break;
    }

    case AST_VAL_CHAR:
    {
        fprintf(output, "%c", value.char_);
        break;
    }

    default:
        break;
    }
}

void scm_print(AstNode node, FILE *output)
{
    if (node.type == AST_VALUE)
    {
        scm_print_value(node.value, output);
    }
    else
    {
        fprintf(output, "nil");
    }
}
