#include <lib/log.h>
#include <scm/lex.h>
#include <scm/parser.h>

AstNode parse_token(int *index, Tokens tokens)
{
    AstNode ret = {0};

    ret.value.raw = malloc(100);

    Token token = tokens.data[*index];

    if (token.type == TOKEN_QUOTE)
    {
        *index += 1;

        token = tokens.data[*index];

        ret.value.quoted = true;
    }

    switch (token.type)
    {

    case TOKEN_LPAREN:
    {
        ret.type = AST_VALUE;
        ret.value.type = AST_VAL_LIST;
        vec_init(&ret.value.list);

        while (tokens.data[++*index].type != TOKEN_RPAREN)
        {
            vec_push(&ret.value.list, parse_token(index, tokens));
        }

        ret.value.raw = "list";

        break;
    }

    case TOKEN_RPAREN:
    {
        error("this shouldn't happen");
        break;
    }

    case TOKEN_CHAR:
    {
        ret.type = AST_VALUE;
        ret.value.type = AST_VAL_CHAR;
        ret.value.char_ = token.char_;
        ret.value.raw = (char[]){token.char_, 0};
        break;
    }

    case TOKEN_NUMBER:
    {
        ret.type = AST_VALUE;
        ret.value.type = AST_VAL_INT;
        ret.value.int_ = token.num;

        sprintf(ret.value.raw, "%d", token.num);
        break;
    }

    case TOKEN_SYMBOL:
    {
        ret.type = AST_VALUE;
        ret.value.type = AST_VAL_SYMBOL;
        ret.value.symbol = token.symbol;
        ret.value.raw = token.symbol;

        break;
    }

    case TOKEN_STRING:
    {
        ret.type = AST_VALUE;
        ret.value.type = AST_VAL_STR;
        ret.value.str = token.string;
        sprintf(ret.value.raw, "\"%s\"", token.string);
        break;
    }

    default:
        break;
    }

    return ret;
}

Ast parse(Tokens tokens)
{
    Ast ret = {0};

    for (int i = 0; i < tokens.length; i++)
    {
        vec_push(&ret, parse_token(&i, tokens));
    }

    return ret;
}