#include "ast/ast.h"
#include "ast/str.h"
#include "lexer/lexer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int arr_cap = 128;

static lexer *l;
static token curr_token;
static token next_token;

static header parse_expression(precedence);
static statement parse_statement();

typedef header (*parse_fn)();
typedef header (*parse_inflix_fn)(header);

static void add_statement(statements *sts, statement s)
{
    sts->sts[sts->len] = s;
    sts->len++;
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

static void print_let_exp(str *buf, exp *e)
{
    // exp_identifier *e = exp;
    // str_appendf(buf, "%s", e->value);
    str_appendf(buf, "%s", "TODO PRINT LET EXP");
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

static void print_exp_integer(str *buf, exp *exp)
{
    exp_integer *e = exp;
    str_appendf(buf, "%d", e->value);
}

static void print_boolean_exp(str *buf, exp *exp)
{
    exp_boolean *e = exp;
    if (e->value == true)
        str_appendf(buf, "true");
    else
        str_appendf(buf, "false");
}

static void print_inflix_exp(str *buf, exp *exp)
{
    exp_inflix *e = exp;
    str_appendf(buf, "(");
    e->left.print_fn(buf, e->left.exp);
    str_appendf(buf, " %s ", e->op);
    e->right.print_fn(buf, e->right.exp);
    str_appendf(buf, ")");
}

static void print_prefix_exp(str *buf, exp *exp)
{
    exp_prefix *e = exp;
    str_appendf(buf, "(");
    str_appendf(buf, "%s", e->op);
    e->right.print_fn(buf, e->right.exp);
    str_appendf(buf, ")");
}

static void print_if_exp(str *buf, exp *exp)
{
    str_appendf(buf, "TODO IF PRINTIN");
}

static void print_ident_exp(str *buf, exp *exp)
{
    exp_identifier *e = exp;
    str_appendf(buf, "%s", e->value);
}

static int parse_let_statement(statement *s)
{
    s->token = curr_token;
    s->literal = strdup(curr_token.literal);

    if (!expect_next_token(T_IDENT))
        return error("failed T_IDENT check");

    tokens_advance();
    let_statement *e = malloc(sizeof(let_statement));
    e->token = curr_token;
    e->name = strdup(curr_token.literal);

    if (!expect_next_token(T_ASSIGN))
        return error("failed T_ASSIGN check");

    tokens_advance(); // =
    tokens_advance(); // expression

    e->value = parse_expression(P_LOWEST);

    s->exp = (header){
        .print_fn = &print_let_exp,
        .exp = e,
    };

    if (next_token.token == T_SEMICOLON)
        tokens_advance();

    return -1;
}

static int parse_return_statement(statement *s)
{
    s->token = curr_token;
    s->literal = strdup(curr_token.literal);

    tokens_advance();
    return_statement *e = malloc(sizeof(return_statement));
    e->token = curr_token;
    e->value = parse_expression(P_LOWEST);

    // TODO: check for EOF
    while (curr_token.token != T_SEMICOLON && !l->eof)
        tokens_advance();

    return -1;
}

static header parse_integer_expression()
{
    exp_integer *e = malloc(sizeof(exp_integer));
    e->token = curr_token;
    e->value = strtol(curr_token.literal, NULL, 10);
    return (header){
        .print_fn = &print_exp_integer,
        .exp = e,
    };
}

static header parse_inflix_expression(header h)
{
    exp_inflix *e = malloc(sizeof(exp_inflix));
    e->token = curr_token;
    e->op = strdup(curr_token.literal);
    e->left = h;

    precedence p = precedence_by_token(curr_token);
    tokens_advance();
    e->right = parse_expression(p);
    return (header){
        .print_fn = &print_inflix_exp,
        .exp = e,
    };
}

static header parse_prefix_expression()
{
    exp_prefix *e = malloc(sizeof(exp_prefix));
    e->token = curr_token;
    e->op = strdup(curr_token.literal);
    tokens_advance();
    e->right = parse_expression(P_PREFIX);
    return (header){
        .print_fn = &print_prefix_exp,
        .exp = e,
    };
}

static header parse_ident_expression()
{
    exp_identifier *e = malloc(sizeof(exp_identifier));
    e->token = curr_token;
    e->value = strdup(curr_token.literal);
    return (header){
        .print_fn = &print_ident_exp,
        .exp = e,
    };
}

static header parse_boolean_expression()
{
    exp_boolean *e = malloc(sizeof(exp_boolean));
    e->token = curr_token;
    e->value = (strcmp(curr_token.literal, "true") == 0);
    return (header){
        .print_fn = &print_boolean_exp,
        .exp = e,
    };
}

static header parse_group_expression()
{
    tokens_advance();

    header exp = parse_expression(P_LOWEST);

    if (!expect_next_token(T_RPAREN))
        return (header){.exp = NULL};

    tokens_advance();
    return exp;
}

static statements parse_block()
{
    statements sts = {
        .len = 0,
        .sts = malloc(sizeof(statement) * arr_cap),
    };

    for (;;) {
        if (l->eof)
            break;
        if (curr_token.token == T_RBRACE) {
            tokens_advance();
            break;
        }

        statement s = parse_statement();
        if (s.token.token == 0)
            return sts;

        add_statement(&sts, s);
        tokens_advance();
    }

    return sts;
}
static header parse_if_expression()
{
    tokens_advance();

    exp_if *e = malloc(sizeof(exp_if));
    e->token = curr_token;

    // TODO: check for null
    header cond_exp = parse_expression(P_LOWEST);
    e->condition = cond_exp;

    if (curr_token.token != T_RPAREN)
        return (header){.exp = NULL};

    tokens_advance();

    if (curr_token.token != T_LBRACE)
        return (header){.exp = NULL};

    tokens_advance();

    e->consequence = parse_block();

    if (curr_token.token == T_ELSE) {
        tokens_advance();
        tokens_advance();
        e->alternative = parse_block();
    }

    return (header){
        .print_fn = &print_if_exp,
        .exp = e,
    };
}

static parse_fn get_parse_prefix_fn()
{
    switch (curr_token.token) {
    case T_IDENT:
        return &parse_ident_expression;
    case T_INT:
        return &parse_integer_expression;
    case T_MINUS:
    case T_BANG:
        return &parse_prefix_expression;
    case T_TRUE:
    case T_FALSE:
        return &parse_boolean_expression;
    case T_LPAREN:
        return &parse_group_expression;
    case T_IF:
        return &parse_if_expression;
    default:
        return NULL;
    }
}

static parse_inflix_fn get_parse_inflix_fn(token t)
{
    switch (t.token) {
    case T_PLUS:
    case T_MINUS:
    case T_SLASH:
    case T_ASTERISK:
    case T_EQ:
    case T_NOT_EQ:
    case T_LT:
    case T_GT:
        return &parse_inflix_expression;
    default:
        return NULL;
    }
}

static header parse_expression(precedence p)
{
    parse_fn prefix_fn = get_parse_prefix_fn();
    if (prefix_fn == NULL) {
        printf("prefix_fn func not found for '%s'\n", curr_token.literal);
        abort();
    }

    header leftExp = prefix_fn();

    while (next_token.token != T_SEMICOLON && p < precedence_by_token(next_token)) {
        parse_inflix_fn inflix_fn = get_parse_inflix_fn(next_token);
        if (inflix_fn == NULL)
            return leftExp;

        tokens_advance();

        leftExp = inflix_fn(leftExp);
    }

    return leftExp;
}

static int parse_expression_statement(statement *s)
{
    s->token = curr_token;
    s->exp = parse_expression(P_LOWEST);

    if (next_token.token == T_SEMICOLON)
        tokens_advance();

    return -1;
}

static statement parse_statement()
{
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
    return s;
}

void print_sts(statements sts)
{
    str *buf = str_new(NULL);
    for (int i = 0; i < sts.len; i++) {
        if (sts.sts[i].exp.print_fn == NULL) {
            printf("token: %s\n", sts.sts[i].token.literal);
            // TODO: print return and let expressions;
            continue;
        }
        sts.sts[i].exp.print_fn(buf, sts.sts[i].exp.exp);

        printf("%s\n", buf->data);
        str_reset(buf);
    }
}

void ast_to_str(str *buf, statements sts)
{
    for (int i = 0; i < sts.len; i++) {
        sts.sts[i].exp.print_fn(buf, sts.sts[i].exp.exp);
    }
}

statements ast_parse(lexer *lex)
{
    l = lex;

    statements result = {
        .len = 0,
        .sts = malloc(sizeof(statement) * arr_cap),
    };

    tokens_advance();
    tokens_advance();

    while (curr_token.token != T_EOF) {
        statement s = parse_statement();

        if (s.token.token > 0)
            add_statement(&result, s);

        tokens_advance();
    }

    return result;
}
