#include <ctype.h>
#include <lex.h>
#include <lib/log.h>
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
    return isdigit(c) || isalpha(c) || c == '_' || c == '-';
}

bool is_digit(char c)
{
    return isdigit(c);
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
            vec_push(&ret, curr);
            s++;

            break;
        }

        case ';':
        {
            s++;

            while (*s != '\n')
                s++;
            break;
        }

        default:
        {
            if (isalpha(*s))
            {
                curr.type = TOKEN_SYMBOL;
                curr._symbol = str_while(&s, is_symbol);
                vec_push(&ret, curr);
            }
            else if (isdigit(*s))
            {

                curr.type = TOKEN_NUMBER;

                bool is_hex = false;

                if ((*s - '0') == 0 && *(s + 1) == 'x')
                {
                    is_hex = true;
                    s += 2;
                }

                char *num = str_while(&s, is_digit);

                sscanf(num, is_hex ? "%x" : "%u", &curr._num);
                vec_push(&ret, curr);
            }

            else if (*s == '#')
            {
                curr.type = TOKEN_REGISTER;
                s++;
                switch (tolower(*s))
                {
                case 'a':
                    curr._register = REG_A;
                    break;

                case 'b':
                    curr._register = REG_B;
                    break;

                case 'c':
                    curr._register = REG_C;
                    break;

                case 'd':
                    curr._register = REG_D;
                    break;

                case 'e':
                    curr._register = REG_E;
                    break;

                case 'f':
                    curr._register = REG_F;
                    break;

                case 'g':
                    curr._register = REG_G;
                    break;

                case 'h':
                    curr._register = REG_H;
                    break;

                case 'p':
                    s++;
                    if (tolower(*s) == 'c')
                    {
                        curr._register = REG_PC;
                    }
                    break;

                case '\\':
                {
                    s++;

                    curr.type = TOKEN_CHAR;
                    curr._char = *s;

                    if (tolower(*s) == 'n')
                    {
                        if (tolower(*(s + 1)) == 'l')
                        {
                            s++;
                            curr._char = '\n';
                        }
                    }

                    break;
                }

                case 's':
                    s++;
                    if (tolower(*s) == 'p')
                    {
                        curr._register = REG_PC;
                    }
                    break;

                default:
                    error("Unknown register: %c\n", *s);
                    exit(-1);
                    break;
                }

                vec_push(&ret, curr);
            }
        }
        }

        s++;
    }

    return ret;
}
