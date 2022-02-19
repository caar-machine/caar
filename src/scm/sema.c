#include "parser.h"
#include <lib/log.h>
#include <scm/sema.h>

static ScmVarMap variables = {0};

ScmType scm_type_from_value(AstValue value)
{

    switch (value.type)
    {

    case AST_VAL_INT:
        return SCM_TYPE_INT;

    case AST_VAL_STR:
        return SCM_TYPE_STR;

    case AST_VAL_CHAR:
        return SCM_TYPE_CHAR;

    case AST_VAL_SYMBOL:
    {
        ScmVarDecl *decl = map_get(&variables, value.symbol);

        if (!decl)
        {
            break;
        }
        return decl->type;
    }

    case AST_VAL_LIST:
    {
        return SCM_TYPE_PROC;
    }

    default:
        error("dunno");
        break;
    }

    return -1;
}

void scm_define_variable(char *name, AstValue value)
{
    map_set(&variables, name, ((ScmVarDecl){.name = strdup(name), .type = scm_type_from_value(value)}));
}

void scm_sema_check_value(AstValue val)
{
    switch (val.type)
    {

    case AST_VAL_LIST:
    {
        if (!val.quoted)
        {
            if (val.list.length == 0)
            {
                error("wrong type to apply: 'Nil' ");
                break;
            }

            if (val.list.data[0].value.type != AST_VAL_SYMBOL)
            {
                error("wrong type to apply: %s", val.list.data[0].value.raw);
                break;
            }

            if (!strcmp(val.list.data[0].value.symbol, "define"))
            {

                if (val.list.data[1].value.type == AST_VAL_LIST)
                {
                    if (val.list.length < 3)
                    {
                        error("expected body in procedure definition");
                        break;
                    }

                    scm_define_variable(val.list.data[1].value.list.data[0].value.symbol, val.list.data[1].value);
                }

                else if (val.list.data[1].value.type != AST_VAL_SYMBOL)
                {
                    error("Expected symbol in variable definition");
                    break;
                }

                else
                {

                    scm_sema_check_value(val.list.data[2].value);

                    scm_define_variable(val.list.data[1].value.symbol, val.list.data[2].value);
                }

                info("Variable definition: %s with value %s", val.list.data[1].value.symbol, val.list.data[2].value.raw);
            }

            else
            {
                scm_sema_check_value(val.list.data[0].value);
            }
        }

        break;
    }

    case AST_VAL_SYMBOL:
    {
        ScmVarDecl *decl = map_get(&variables, val.symbol);

        if (!decl)
        {
            error("Undefined symbol: %s", val.symbol);
            break;
        }

        break;
    }

    default:
        break;
    }
}
void scm_sema_check_node(AstNode node)
{
    if (node.type == AST_VALUE)
    {
        scm_sema_check_value(node.value);
    }
}

void scm_sema_check(Ast ast)
{
    for (int i = 0; i < ast.length; i++)
    {
        scm_sema_check_node(ast.data[i]);
    }
}