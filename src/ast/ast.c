#include "ast/ast.h"
#include "lexer/lexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int arr_cap = 128;

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

static bool expect_next_token(token_t t)
{
    return next_token.token == t;
}

static int error(char *s)
{
    printf("%s\r\n", s);
    return -1;
}

static int parse_let_statement(statement *s)
{
    s->token = curr_token;

    if (!expect_next_token(T_IDENT))
        return error("expected token to be T_INDENT");

    s->name = strdup(next_token.literal);

    tokens_advance();

    if (!expect_next_token(T_ASSIGN))
        return error("expected token to be T_EQ");

    return -1;
}

static int parse_return_statement(statement *s)
{
    s->token = curr_token;

    // TODO: check for EOF
    while (curr_token.token != T_SEMICOLON)
        tokens_advance();

    return -1;
}

statement *ast_parse(lexer *lex, int *len)
{
    curr_token = (token){0};
    next_token = (token){0};
    arr_len = 0;
    l = lex;

    statement *sts = malloc(sizeof(statement) * arr_cap);
    tokens_advance();

    while (curr_token.token != T_EOF) {
        statement s = {0};

        if (curr_token.token == T_LET) {
            if (!parse_let_statement(&s)) {
                printf("ERROR: failed to parse let statement\r\n");
                abort();
            }
            add_statement(sts, s);
        }
        if (curr_token.token == T_RETURN) {
            if (!parse_return_statement(&s)) {
                printf("ERROR: failed to parse return statement\r\n");
                abort();
            }
            add_statement(sts, s);
        }

        tokens_advance();
    }

    *len = arr_len;
    return sts;
}
