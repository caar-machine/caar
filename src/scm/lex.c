#include <ctype.h>
#include <lib/log.h>
#include <scm/lex.h>
#include <stdbool.h>

#define TOKEN(CHAR, TYPE)     \
    case CHAR:                \
        curr.type = TYPE;     \
        vec_push(&ret, curr); \
        break;

char *str_while(char **s, bool (*callback)(char c))
{
    vec_char_t ret = {0};
    vec_init(&ret);

    while (callback(**s))
    {
        if (!callback(**s))
        {
            break;
        }

        vec_push(&ret, **s);

        *s += 1;
    }
    *s -= 1;
    vec_push(&ret, '\0');

    return ret.data;
}

bool is_symbol(char c)
{
    return isdigit(c) || isalpha(c) || c == '_' ||
           c == '-' ||
           c == '+' ||
           c == '/' || c == '*' ||
           c == '>' || c == '<' || c == '=' || c == '?';
}

bool is_digit(char c)
{
    return isdigit(c) || isxdigit(c);
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
            TOKEN('\'', TOKEN_QUOTE);

        case '"':
        {
            s++;
            curr.type = TOKEN_STRING;

            curr.string = str_while(&s, is_not_quote);
            vec_push(&ret, curr);
            s++;

            break;
        }

        case ';':
        {
            s++;

            while (*s != '\n')
            {
                if (*s == 0)
                {
                    break;
                }

                s++;
            }

            break;
        }

        case '#':
        {
            bool is_hex = false, is_char = false;

            if (*(s + 1) == 'x')
            {
                is_hex = true;

                s += 2;
            }

            else if (*(s + 1) == '\\')
            {
                is_char = true;
                s += 2;
            }

            if (is_hex)
            {
                curr.type = TOKEN_NUMBER;

                char *num = str_while(&s, is_digit);

                sscanf(num, "%x", &curr.num);
            }

            if (is_char)
            {
                curr.type = TOKEN_CHAR;
                curr.char_ = *s;
            }

            vec_push(&ret, curr);

            break;
        }

        default:
        {
            if (isalpha(*s) || *s == '+' || *s == '-' || *s == '/' || *s == '*' ||
                *s == '=' || *s == '>' || *s == '<')
            {
                curr.type = TOKEN_SYMBOL;

                curr.symbol = str_while(&s, is_symbol);

                vec_push(&ret, curr);
            }

            else if (isdigit(*s))
            {

                curr.type = TOKEN_NUMBER;

                char *num = str_while(&s, is_digit);

                sscanf(num, "%u", &curr.num);

                vec_push(&ret, curr);
            }

            else
            {
                break;
            }
        }
        }

        s++;
    }

    return ret;
}
