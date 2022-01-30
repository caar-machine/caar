#include <ast.h>
#include <lib/log.h>

int get_file_size(FILE *fp);

AstNode parse_token(int *index, Ast *ast, Tokens tokens)
{
    AstNode node = {0};

    switch (tokens.data[*index].type)
    {

    case TOKEN_LPAREN:
    {
        node.type = AST_CALL;

        if (*index + 1 > tokens.length || tokens.data[*index + 1].type != TOKEN_SYMBOL)
        {
            error("expected symbol in instruction");
            exit(-1);
        }

        node.call.name = tokens.data[++*index]._symbol;

        if (!strcmp(node.call.name, "include"))
        {
            FILE *fp = fopen(tokens.data[ *index += 1]._string, "r");

            if (!fp)
            {
                error("could not open file: %s", tokens.data[*index]._string);
                exit(-1);
            }

            int size = get_file_size(fp);

            char *text = calloc(size, 1);

            fread(text, 1, size, fp);

            fclose(fp);

            Tokens new_tokens = lex(text);

            for (int i = 0; i < new_tokens.length; i++)
            {
                vec_push(ast, parse_token(&i, ast, new_tokens));
            }

            *index += 1;

            free(text);
        }

        else if (!strcmp(node.call.name, "defmacro"))
        {
            node.type = AST_MACRO;
            node.macro.name = tokens.data[ *index += 1]._symbol;

            *index += 2;

            Token tok = tokens.data[*index];

            while (tok.type != TOKEN_RPAREN)
            {
                tok = tokens.data[*index];

                if (tok.type != TOKEN_RPAREN)
                {
                    vec_push(&node.macro.params, tok._symbol);
                }

                else
                    continue;

                *index += 1;
            }

            Ast ret = {0};

            while (tokens.data[++*index].type != TOKEN_RPAREN)
            {
                vec_push(&ret, parse_token(index, ast, tokens));
            }

            node.macro.body = ret;
        }

        else
        {
            if (*index + 1 > tokens.length)
            {
                error("expected ')' at end of instruction");
                exit(-1);
            }

            while (tokens.data[++*index].type != TOKEN_RPAREN)
            {
                vec_push(&node.call.params, parse_token(index, ast, tokens).value);
            }
        }

        break;
    }

    case TOKEN_NUMBER:
    {
        node.type = AST_VALUE;
        node.value.type = AST_VAL_INT;
        node.value.int_ = tokens.data[*index]._num;
        break;
    }

    case TOKEN_STRING:
    {
        node.type = AST_VALUE;
        node.value.type = AST_VAL_STR;
        node.value.str_ = tokens.data[*index]._string;
        break;
    }

    case TOKEN_CHAR:
    {
        node.type = AST_VALUE;
        node.value.type = AST_VAL_INT;
        node.value.int_ = (int)tokens.data[*index]._char;
        break;
    }

    case TOKEN_SYMBOL:
    {
        node.type = AST_VALUE;
        node.value.type = AST_VAL_SYMBOL;
        node.value.str_ = tokens.data[*index]._symbol;
        break;
    }

    case TOKEN_REGISTER:
    {
        node.type = AST_VALUE;
        node.value.type = AST_VAL_REG;
        node.value.reg_ = tokens.data[*index]._register;
        break;
    }

    case TOKEN_RPAREN:
    case TOKEN_LBRACKET:
    case TOKEN_RBRACKET:
        error("Unexpected token of type %d at position %d", tokens.data[*index].type, *index);
        exit(-1);
        break;

    default:
        error("this shouldn't happen. please open an issue at https://github.com/Abb1x/caar");

        exit(-1);
        break;
    }

    return node;
}

Ast parse(Tokens tokens)
{
    Ast ast = {0};
    vec_init(&ast);

    for (int i = 0; i < tokens.length; i++)
    {
        AstNode curr = parse_token(&i, &ast, tokens);

        vec_push(&ast, curr);
    }

    return ast;
}

Ast astvalues_to_ast(AstValues values)
{
    Ast ast = {0};
    vec_init(&ast);

    for (int i = 0; i < values.length; i++)
    {
        AstNode node = {0};
        node.type = AST_VALUE;
        node.value = values.data[i];
        vec_push(&ast, node);
    }

    return ast;
}
