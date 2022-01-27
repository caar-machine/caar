#include <SDL2/SDL_keycode.h>
#include <ast.h>

// FIXME: fix this horrible code pls

AstNode parse_token(int *index, Tokens tokens)
{
    AstNode node = {0};

    switch (tokens.data[*index].type)
    {

    case TOKEN_LPAREN:
    {
        node.type = AST_CALL;

        if (tokens.data[*index + 1].type != TOKEN_SYMBOL)
        {
            printf("ERROR: expected symbol in function call\n");
        }

        node.call.name = tokens.data[++*index]._symbol;

        while (tokens.data[*index].type != TOKEN_RPAREN)
        {
            vec_push(&node.call.params, parse_token(index, tokens).value);
            *index += 1;
        }
        break;
    }

    case TOKEN_NUMBER:
    {
        node.type = AST_VALUE;
        node.value.int_ = tokens.data[*index]._num;
        break;
    }

    default:
        printf("todo: %d\n", tokens.data[*index].type);

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