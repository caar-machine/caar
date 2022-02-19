#ifndef SCM_PRINT_H
#define SCM_PRINT_H
#include <scm/parser.h>
#include <stdio.h>

void scm_print(AstNode node, FILE *output);
void scm_print_value(AstValue value, FILE *output);
void scm_print_list(Ast list, FILE *output);

#endif