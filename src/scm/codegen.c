#include "sema.h"
#include <lib/log.h>
#include <scm/codegen.h>
#include <scm/parser.h>
#include <scm/print.h>

void codegen_value(AstValue value, FILE *output)
{
    (void)output;
    switch (value.type)
    {
    case AST_VAL_LIST:
    {
        if (!value.quoted)
        {
        }

        break;
    }

    default:
        break;
    }

}

void hello(){

}

void scm_compile(char *input, FILE *output)
{
    Tokens tokens = lex(input);
    Ast ast = parse(tokens);

    scm_sema_check(ast);

    for (int i = 0; i < ast.length; i++)
    {
        if (ast.data[i].type == AST_VALUE)
        {
            codegen_value(ast.data[i].value, output);
        }

        fprintf(stdout, "=> ");

        scm_print(ast.data[i], stdout);

        fprintf(stdout, "\n");
    }
}
