#ifndef AS_LEX_H
#define AS_LEX_H
#include <lib/vec.h>

typedef enum
{
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_SYMBOL,
} TokenType;

typedef struct
{
    TokenType type;

    union
    {
        char *_string;
        int _num;
        char *_symbol;
    };

} Token;

typedef vec_t(Token) Tokens;

Tokens lex(char *s);

#endif