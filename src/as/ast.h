#ifndef AS_AST_H
#define AS_AST_H
#include <lex.h>
#include <lib/map.h>

typedef enum
{
    AST_CALL,
    AST_VALUE,
    AST_MACRO,
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

typedef struct AstNode AstNode;

typedef vec_t(AstNode) Ast;

typedef struct
{
    char *name;
    vec_str_t params;
    Ast body;
} AstMacro;

typedef struct AstNode
{
    AstNodeType type;

    union
    {
        AstCall call;
        AstValue value;
        AstMacro macro;
    };

} AstNode;

Ast parse(Tokens tokens);
Ast astvalues_to_ast(AstValues value);

#endif
