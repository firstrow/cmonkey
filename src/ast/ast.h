#ifndef AST_H
#define AST_H

#include "lexer/lexer.h"

typedef void exp;

typedef struct
{
    void (*print_fn)(exp *);
    exp *exp; // hold expression: exp_integer, etc...
} header;

typedef struct
{
    token token;
    char *literal;
    header exp;
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
    header right;
} exp_prefix;

typedef struct
{
    token token;
    header left;
    char *op;
    header right;
} exp_inflix;

statement *ast_parse(lexer *l, int *len);
void print_sts(statement *sts, int len);

#endif // AST_H
