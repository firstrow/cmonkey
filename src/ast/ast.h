#ifndef AST_H
#define AST_H

#include "lexer/lexer.h"

typedef struct
{
    token token;
    char *literal;
    void *exp;
} statement;

typedef struct {
    token token;
    char *value;
} exp_identifier;

statement *ast_parse(lexer *l, int *len);
void print_sts(statement *sts, int len);

#endif // AST_H
