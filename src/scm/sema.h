#ifndef SCM_SEMA_H
#define SCM_SEMA_H
#include <lib/map.h>
#include <scm/parser.h>

typedef enum
{
    SCM_TYPE_LIST,
    SCM_TYPE_PROC,
    SCM_TYPE_INT,
    SCM_TYPE_STR,
    SCM_TYPE_BOOL,
    SCM_TYPE_SYMBOL,
    SCM_TYPE_CHAR,
} ScmType;

typedef struct
{
    ScmType type;
    char *name;
} ScmVarDecl;

typedef map_t(ScmVarDecl) ScmVarMap;

void scm_sema_check(Ast ast);

#endif