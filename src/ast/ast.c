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

    s->literal = strdup(next_token.literal);

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

static int parse_expression_statement(statement *s)
{
    switch (curr_token.token) {
    case T_IDENT:
        s->token = curr_token;
        exp_identifier *e1 = malloc(sizeof(exp_identifier));
        e1->token = curr_token;
        e1->value = strdup(curr_token.literal);
        s->exp = e1;
        break;
    case T_INT:
        s->token = curr_token;
        exp_integer *e = malloc(sizeof(exp_integer));
        e->value = strtol(curr_token.literal, NULL, 10);
        s->exp = e;
        break;
    default:
        break;
    }

    if (next_token.token == T_SEMICOLON) {
        tokens_advance();
    }

    return -1;
}

void print_sts(statement *sts, int len)
{
    printf(">>>>>>>>>>>>>>>>>>>>>\r\n");
    for (int i = 0; i < len; i++) {
        printf("token: %d\r\n", sts[i].token.token);
        printf("literal: %s\r\n", sts[i].token.literal);
        printf("_____________________\r\n");
    }
    printf(">>>>>>>>>>>>>>>>>>>>>\r\n");
}

statement *ast_parse(lexer *lex, int *len)
{
    arr_len = 0;
    l = lex;
    statement *sts = malloc(sizeof(statement) * arr_cap);
    tokens_advance();
    tokens_advance();

    while (curr_token.token != T_EOF) {
        statement s = {0};

        switch (curr_token.token) {
        case T_LET:
            if (!parse_let_statement(&s)) {
                printf("ERROR: failed to parse let statement\r\n");
                abort();
            }
            break;
        case T_RETURN:
            if (!parse_return_statement(&s)) {
                printf("ERROR: failed to parse return statement\r\n");
                abort();
            }
            break;
        default:
            if (!parse_expression_statement(&s)) {
                printf("ERROR: failed to parse expression statement\r\n");
                abort();
            }
            break;
        }

        if (s.token.token > 0)
            add_statement(sts, s);

        tokens_advance();
    }

    *len = arr_len;
    return sts;
}
