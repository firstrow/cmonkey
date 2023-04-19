#ifndef AST_H
#define AST_H

#include "ast/str.h"
#include "lexer/lexer.h"
#include <stdbool.h>

typedef void exp;

typedef struct
{
    void (*print_fn)(str *buf, exp *);
    exp *exp; // holds expression: exp_integer, etc...
} header;

typedef struct
{
    token token;
    char *literal;
    header exp;
} statement;

typedef struct {
    int len;
    statement *sts;
} statements;

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
    bool value;
} exp_boolean;

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

typedef struct
{
    token token;
    header condition;
    statements consequence;
    statements alternative;
} exp_if;

statements ast_parse(lexer *l);
void print_sts(statements sts);
void ast_to_str(str *buf, statements sts);

#endif // AST_H
