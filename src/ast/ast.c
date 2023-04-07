#include "ast/ast.h"
#include "lexer/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int arr_cap;
static int arr_len;
static lexer *l;
static token curr_token;
static token next_token;

static void add_statement(statement *arr, statement s)
{
    arr[arr_len] = s;
    arr_len++;
}

static void tokens_advance()
{
    curr_token = next_token;
    next_token = lexer_next_token(l);
}

static int parse_let_statement(statement *s)
{
    s->token = curr_token;

    if (next_token.token != T_IDENT)
        return 1;

    s->name = strdup(next_token.literal);

    tokens_advance();

    if (next_token.token != T_EQ)
        return 1;

    return -1;
}

// len: total number of statements found
statement *ast_parse(lexer *lex, int *len)
{
    arr_len = 0;
    l = lex;

    statement *sts = malloc(sizeof(statement) * arr_cap);
    tokens_advance();

    while (next_token.token != T_EOF) {
        statement s = {0};

        if (curr_token.token == T_LET) {
            if (!parse_let_statement(&s)) {
                printf("ERROR: failed to parse let statement\r\n");
                abort();
            }
            add_statement(sts, s);
        }

        tokens_advance();
    }

    *len = arr_len;
    return sts;
}
