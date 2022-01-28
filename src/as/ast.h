#ifndef AS_AST_H
#define AS_AST_H
#include <lex.h>

typedef enum
{
    AST_CALL,
    AST_VALUE,
} AstNodeType;

typedef struct
{
    enum
    {
        AST_VAL_INT,
        AST_VAL_STR,
        AST_VAL_SYMBOL,
        AST_VAL_REG,
    } type;

    union
    {
        unsigned int int_;
        char *str_;
        char *symbol_;
        Register reg_;
    };

} AstValue;

typedef vec_t(AstValue) AstValues;

typedef struct
{
    char *name;
    AstValues params;
} AstCall;

typedef struct
{
    AstNodeType type;

    union
    {
        AstCall call;
        AstValue value;
    };

} AstNode;

typedef vec_t(AstNode) Ast;

Ast parse(Tokens tokens);
Ast astvalues_to_ast(AstValues value);

#endif