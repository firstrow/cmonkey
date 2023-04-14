#ifndef AST_H
#define AST_H

#include "lexer/lexer.h"

typedef void exp;

typedef struct
{
    token token;
    char *literal;
    exp *exp;
} statement;

// expressions

typedef struct
{
    token token;
    char *value;
} exp_identifier;

typedef struct
{
    token token;
    int value;
} exp_integer;

typedef struct
{
    token token;
    char *op;
    exp *right;
} exp_prefix;

statement *ast_parse(lexer *l, int *len);
void print_sts(statement *sts, int len);

#endif // AST_H
