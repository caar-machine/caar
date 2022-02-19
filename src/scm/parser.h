#ifndef SCM_PARSER_H
#define SCM_PARSER_H
#include <lib/vec.h>
#include <scm/lex.h>
#include <stdbool.h>

typedef struct AstNode AstNode;

typedef enum
{
    AST_VALUE,
} AstNodeType;

typedef vec_t(AstNode) Ast;

typedef struct
{
    enum
    {
        AST_VAL_INT,
        AST_VAL_CHAR,
        AST_VAL_STR,
        AST_VAL_SYMBOL,
        AST_VAL_LIST,
    } type;

    union
    {
        unsigned int int_;
        char *str;
        char char_;
        char *symbol;
        Ast list;
    };

    bool quoted;
    char *raw;

} AstValue;

typedef vec_t(AstValue) AstValues;

typedef struct AstNode
{
    AstNodeType type;

    union
    {
        AstValue value;
    };

} AstNode;

Ast parse(Tokens tokens);

#endif