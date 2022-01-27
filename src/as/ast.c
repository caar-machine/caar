#include <SDL2/SDL_keycode.h>
#include <ast.h>

AstNode parse_token(int *index, Tokens tokens)
{
    AstNode node = {0};

    switch (tokens.data[*index].type)
    {
    case TOKEN_LPAREN:
    {
        node.type = AST_CALL;

        if (*index + 1 > tokens.length || tokens.data[*index + 1].type != TOKEN_SYMBOL)
        {
            fprintf(stderr, "ERROR: expected symbol in function call\n");
            exit(-1);
        }

        node.call.name = tokens.data[++*index]._symbol;

        if (*index + 1 > tokens.length)
        {
            fprintf(stderr, "ERROR: expected character ')' in function call\n");
            exit(-1);
        }

        while (tokens.data[++*index].type != TOKEN_RPAREN)
            vec_push(&node.call.params, parse_token(index, tokens).value);

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

    default:
        switch (tokens.data[*index].type)
        {
        case TOKEN_LPAREN:
            fprintf(stderr, "ERROR: unexpected character '('\n");
            break;

        case TOKEN_RPAREN:
            fprintf(stderr, "ERROR: unexpected character ')'\n");
            break;

        case TOKEN_LBRACKET:
            fprintf(stderr, "ERROR: unexpected character '['\n");
            break;

        case TOKEN_RBRACKET:
            fprintf(stderr, "ERROR: unexpected character ']'\n");
            break;

        default:
            fprintf(stderr, "ERROR: Unexpected error\n");
            break;
        }

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
        AstNode curr = parse_token(&i, tokens);

        vec_push(&ast, curr);
    }

    return ast;
}