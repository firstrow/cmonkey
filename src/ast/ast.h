#ifndef AST_H
#define AST_H

#include "lexer/lexer.h"

typedef struct
{
    token token;
    char *name;
} statement;

statement *ast_parse(lexer *l, int *len);

#endif // AST_H
