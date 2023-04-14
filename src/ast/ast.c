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

static exp *parse_expression(precedence);

typedef exp *(*parse_fn)();

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

    tokens_advance();

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

static precedence precedence_by_token(token t)
{
    switch (t.token) {
    case T_EQ:
    case T_NOT_EQ:
        return P_EQUALS;
    case T_LT:
    case T_GT:
        return P_LESSGREATER;
    case T_PLUS:
    case T_MINUS:
        return P_SUM;
    case T_SLASH:
    case T_ASTERISK:
        return P_PRODUCT;
    default:
        return P_LOWEST;
    }
}

static precedence curr_precedence()
{
    return precedence_by_token(curr_token);
}

static precedence peek_precedence()
{
    return precedence_by_token(next_token);
}

static exp *parse_prefix_expression()
{
    exp_prefix *e = malloc(sizeof(exp_prefix));
    e->token = curr_token;
    e->op = strdup(curr_token.literal);
    tokens_advance();
    e->right = parse_expression(P_PREFIX);
    return e;
}

static exp *parse_integer_expression()
{
    exp_integer *e = malloc(sizeof(exp_integer));
    e->token = curr_token;
    e->value = strtol(curr_token.literal, NULL, 10);
    return e;
}

static exp *parse_inflix_expression(exp *left)
{
    exp_inflix *e = malloc(sizeof(exp_inflix));
    e->token = curr_token;
    e->op = strdup(curr_token.literal);
    e->left = left;

    tokens_advance();

    e->right = parse_expression(P_LOWEST);

    return e;
}

static exp *parse_ident_expression()
{
    exp_identifier *e = malloc(sizeof(exp_identifier));
    e->token = curr_token;
    e->value = strdup(curr_token.literal);
    return e;
}

static parse_fn get_parse_fn()
{
    switch (curr_token.token) {
    case T_IDENT:
        return &parse_ident_expression;
    case T_INT:
        return &parse_integer_expression;
    case T_MINUS:
    case T_BANG:
        return &parse_prefix_expression;
    default:
        return NULL;
    }
}

static exp *parse_expression(precedence p)
{
    parse_fn prefix = get_parse_fn();
    if (prefix == NULL) {
        printf("prefix func not for %s found\n", curr_token.literal);
        abort();
    }

    return prefix();

    // token t = curr_token;
    // exp *left = parse_expression(P_LOWEST);
    // if (left == NULL)
    //     return -1;

    // s->token = t;

    // while (curr_token.token != T_SEMICOLON && p < peek_precedence()) {
    //     if (!is_next_token_iflix()) {
    //         s->exp = left;
    //         return -1;
    //     }

    //     tokens_advance();
    //     left = parse_inflix_expression(left);
    // }
    // s->exp = left;
}

static int parse_expression_statement(statement *s, precedence p)
{
    s->token = curr_token;
    s->exp = parse_expression(P_LOWEST);

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
            if (!parse_expression_statement(&s, P_PREFIX)) {
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
