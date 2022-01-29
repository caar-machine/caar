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
    TOKEN_CHAR,
    TOKEN_REGISTER,
} TokenType;

typedef enum
{
    REG_A,
    REG_B,
    REG_C,
    REG_D,
    REG_E,
    REG_F,
    REG_G,
    REG_H,
    REG_PC,
    REG_SP,
} Register;

typedef struct
{
    TokenType type;

    union
    {
        char *_string;
        unsigned int _num;
        char *_symbol;
        Register _register;
        char _char;
    };

} Token;

typedef vec_t(Token) Tokens;

Tokens lex(char *s);

#endif