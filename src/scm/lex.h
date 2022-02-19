#ifndef SCM_LEX_H
#define SCM_LEX_H
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
    TOKEN_QUOTE,
} TokenType;

typedef struct token Token;

typedef struct token
{
    TokenType type;

    union
    {
        char *string;
        unsigned int num;
        char *symbol;
        char char_;
        char *quote;
    };

} Token;

typedef vec_t(Token) Tokens;

Tokens lex(char *s);

#endif
