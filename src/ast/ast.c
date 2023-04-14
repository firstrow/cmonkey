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

static exp *parse_expression();

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

static bool is_next_token_iflix()
{
    switch (next_token.token) {
    case T_PLUS:
    case T_MINUS:
    case T_SLASH:
    case T_ASTERISK:
    case T_EQ:
    case T_NOT_EQ:
    case T_LT:
    case T_GT:
        return true;
        break;
    default:
        return false;
        break;
    }

    return false;
}

static exp *parse_inflix_expression(exp *left)
{
    exp_inflix *e = malloc(sizeof(exp_inflix));
    e->token = curr_token;
    e->op = strdup(curr_token.literal);
    e->left = left;

    tokens_advance();

    e->right = parse_expression();

    return e;
}

static exp *parse_expression()
{
    if (curr_token.token == T_IDENT) {
        exp_identifier *e = malloc(sizeof(exp_identifier));
        e->token = curr_token;
        e->value = strdup(curr_token.literal);
        return e;
    }

    if (curr_token.token == T_INT) {
        exp_integer *e = malloc(sizeof(exp_integer));
        e->token = curr_token;
        e->value = strtol(curr_token.literal, NULL, 10);
        return e;
    }

    if (curr_token.token == T_BANG || curr_token.token == T_MINUS) {
        exp_prefix *e = malloc(sizeof(exp_prefix));
        e->token = curr_token;
        e->op = strdup(curr_token.literal);
        tokens_advance();
        e->right = parse_expression();
        return e;
    }

    return NULL;
}

static int parse_expression_statement(statement *s)
{
    token t = curr_token;
    exp *left = parse_expression();
    if (left != NULL) {
        s->token = t;

        while (curr_token.token != T_SEMICOLON) {
            if (is_next_token_iflix()) {
                tokens_advance();
                left = parse_inflix_expression(left);
            } else {
                s->exp = left;
                break;
            }
        }
    }

    if (next_token.token == T_SEMICOLON)
        tokens_advance();

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
