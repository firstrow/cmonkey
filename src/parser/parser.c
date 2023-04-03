#include "parser/parser.h"
#include <stdio.h>
#include <string.h>

const char *const tokens[] = {
    [T_ILLEGAL] = "ILLEGAL",
    [T_EOF] = "EOF",
    [T_INDENT] = "INDENT",
    [T_INT] = "INT",
    [T_ASSIGN] = "=",
    [T_PLUS] = "+",
    [T_COMMA] = ",",
    [T_SEMICOLON] = ";",
    [T_LPAREN] = "(",
    [T_RPAREN] = ")",
    [T_LBRACE] = "{",
    [T_RBRACE] = "}",
    [T_FUNCTION] = "FUNCTION",
    [T_LET] = "LET",
};

lexer parser_new(char *str)
{
    lexer l = {
        .input = strdup(str),
        .len = strlen(str),
        .pos = 0,
        .readPos = 0,
        .ch = 0,
    };
    return l;
}

void lexer_read_char(lexer *l)
{
    if (l->readPos >= l->len) {
        l->ch = 0;
        return;
    }

    l->ch = l->input[l->readPos];
    l->pos = l->readPos;
    l->readPos += 1;
}
