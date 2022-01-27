#include <ctype.h>
#include <lex.h>
#include <stdbool.h>

#define TOKEN(CHAR, TYPE) \
    case CHAR:            \
        curr.type = TYPE; \
        break;

char *str_while(char **s, bool (*callback)(char c))
{
    vec_char_t ret = {0};
    vec_init(&ret);

    while (callback(**s))
    {
        vec_push(&ret, **s);
        *s += 1;
    }

    return ret.data;
}

bool is_symbol(char c)
{
    return isdigit(c) || isalpha(c) || c == '_' || c == '-';
}

bool is_digit(char c)
{
    return isdigit(c) > 0;
}

bool is_not_quote(char c)
{
    return c != '"';
}

Tokens lex(char *s)
{
    Tokens ret = {};

    vec_init(&ret);

    while (*s)
    {
        Token curr = {0};

        switch (*s)
        {
            TOKEN('(', TOKEN_LPAREN);
            TOKEN('[', TOKEN_LBRACKET);
            TOKEN(')', TOKEN_RPAREN);
            TOKEN(']', TOKEN_RBRACKET);

        case '"':
        {
            s++;
            curr.type = TOKEN_STRING;

            curr._string = str_while(&s, is_not_quote);
            s++;
            break;
        }

        default:
        {
            if (isalpha(*s))
            {
                curr.type = TOKEN_SYMBOL;
                curr._symbol = str_while(&s, is_symbol);
            }

            else if (isdigit(*s))
            {
                curr.type = TOKEN_NUMBER;

                char *num = str_while(&s, is_digit);

                sscanf(num, "%d", &curr._num);
            }
        }
        }

        s++;
        vec_push(&ret, curr);
    }

    return ret;
}