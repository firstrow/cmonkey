#include "lexer/lexer.h"
#include <assert.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/ast.h"
#include "ast/str.h"

static void test_lexer_read_char()
{

    // = = 61
    // + = 43
    // ( = 40
    // ) = 41
    // { = 123
    // } = 125
    // , = 44
    // ; = 59
    char *input = "=+(){},;";
    lexer l = lexer_new(input);

    lexer_read_char(&l), assert(l.ch == 61);
    lexer_read_char(&l), assert(l.ch == 43);
    lexer_read_char(&l), assert(l.ch == 40);
    lexer_read_char(&l), assert(l.ch == 41);
    lexer_read_char(&l), assert(l.ch == 123);
    lexer_read_char(&l), assert(l.ch == 125);
    lexer_read_char(&l), assert(l.ch == 44);
    lexer_read_char(&l), assert(l.ch == 59);
    lexer_read_char(&l), assert(l.ch == 0);
    lexer_read_char(&l), assert(l.ch == 0);
}

static void test_lexer_next_token()
{
    char *input = "let five = 5;     \n\
        let ten = 10;                \n\
        let add = fn(x, y) {         \n\
            x + y;                   \n\
        };                           \n\
        let result = add(five, ten); \n\
        ";
    lexer l = lexer_new(input);

    token t = lexer_next_token(&l);
    assert(t.token == T_LET);

    t = lexer_next_token(&l);
    assert(t.token == T_IDENT);
    assert(strcmp(t.literal, "five") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_ASSIGN);

    t = lexer_next_token(&l);
    assert(t.token == T_INT);
    assert(strcmp(t.literal, "5") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_SEMICOLON);

    t = lexer_next_token(&l);
    assert(t.token == T_LET);

    t = lexer_next_token(&l);
    assert(t.token == T_IDENT);
    assert(strcmp(t.literal, "ten") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_ASSIGN);

    t = lexer_next_token(&l);
    assert(t.token == T_INT);
    assert(strcmp(t.literal, "10") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_SEMICOLON);

    t = lexer_next_token(&l);
    assert(t.token == T_LET);
}

static void test_lexer_more_operators()
{
    char *input = "=+-!*/<>";
    lexer l = lexer_new(input);

    token t = lexer_next_token(&l);
    assert(t.token == T_ASSIGN);

    t = lexer_next_token(&l);
    assert(t.token == T_PLUS);

    t = lexer_next_token(&l);
    assert(t.token == T_MINUS);
}

static void test_if_else_return()
{
    char *input = "if (5 < 10) {   \n\
            return true;           \n\
        } else {                   \n\
            return false;          \n\
        }                          \n\
        ";
    lexer l = lexer_new(input);

    token t = lexer_next_token(&l);
    assert(t.token == T_IF);

    t = lexer_next_token(&l);
    assert(t.token == T_LPAREN);
    assert(strcmp(t.literal, "(") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_INT);
    assert(strcmp(t.literal, "5") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_LT);
    assert(strcmp(t.literal, "<") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_INT);
    assert(strcmp(t.literal, "10") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_RPAREN);
    assert(strcmp(t.literal, ")") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_LBRACE);
    assert(strcmp(t.literal, "{") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_RETURN);
    assert(strcmp(t.literal, "return") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_TRUE);
    assert(strcmp(t.literal, "true") == 0);
}

static void test_eq_not_eq()
{
    char *input = "5 == 10   \n\
                   1 != 1;   \n\
                  ";
    lexer l = lexer_new(input);

    token t = lexer_next_token(&l);
    assert(t.token == T_INT);
    assert(strcmp(t.literal, "5") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_EQ);
    assert(strcmp(t.literal, "==") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_INT);
    assert(strcmp(t.literal, "10") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_INT);
    assert(strcmp(t.literal, "1") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_NOT_EQ);
    assert(strcmp(t.literal, "!=") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_INT);
    assert(strcmp(t.literal, "1") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_SEMICOLON);
    assert(strcmp(t.literal, ";") == 0);

    t = lexer_next_token(&l);
    assert(t.token == T_EOF);
    assert(strcmp(t.literal, "") == 0);
}

static void test_ast_let_statement()
{
    char *input = "       \n\
        let x = 5;        \n\
        let y = 10;       \n\
        let foobar = 123; \n\
        ";

    int i;
    lexer l = lexer_new(input);
    statement *sts = ast_parse(&l, &i);

    // print_sts(sts, i);

    assert(i == 6);
    assert(sts[0].token.token == T_LET);
    assert(strcmp(sts[0].literal, "x") == 0);

    assert(sts[2].token.token == T_LET);
    assert(strcmp(sts[2].literal, "y") == 0);

    assert(sts[4].token.token == T_LET);
    assert(strcmp(sts[4].literal, "foobar") == 0);

    free(sts);
}

static void test_ast_return_statement()
{
    char *input = "      \n\
        return 5;        \n\
        return 10;       \n\
        return 123;      \n\
        ";

    int i;
    lexer l = lexer_new(input);
    statement *sts = ast_parse(&l, &i);

    assert(i == 3);
    assert(sts[0].token.token == T_RETURN);
    assert(sts[1].token.token == T_RETURN);
    assert(sts[2].token.token == T_RETURN);

    free(sts);
}

static void test_identifier_expression()
{
    char *input = "foobar;";

    int i;
    lexer l = lexer_new(input);
    statement *sts = ast_parse(&l, &i);

    assert(i == 1);
    assert(sts[0].token.token == T_IDENT);

    exp_identifier *exp = (exp_identifier *)sts[0].exp.exp;
    assert(strcmp(exp->value, "foobar") == 0);

    free(sts);
}

static void test_integer_expression()
{
    char *input = "5;";

    int i;
    lexer l = lexer_new(input);
    statement *sts = ast_parse(&l, &i);

    assert(i == 1);
    assert(sts[0].token.token == T_INT);
    exp_integer *exp = sts[0].exp.exp;
    assert(exp->value == 5);

    free(sts);
}

static void test_prefix_expression()
{
    char *input = " \n\
        !5;         \n\
        -15;        \n\
        ";

    int i;
    lexer l = lexer_new(input);
    statement *sts = ast_parse(&l, &i);

    // print_sts(sts, i);

    assert(i == 2);
    assert(sts[0].token.token == T_BANG);
    exp_prefix *exp = sts[0].exp.exp;
    exp_integer *right_exp = exp->right.exp;
    assert(strcmp(exp->op, "!") == 0);
    assert(right_exp->token.token == T_INT);
    assert(right_exp->value == 5);

    assert(sts[1].token.token == T_MINUS);
    exp = sts[1].exp.exp;
    right_exp = exp->right.exp;
    assert(strcmp(exp->op, "-") == 0);
    assert(right_exp->token.token == T_INT);
    assert(right_exp->value == 15);

    free(sts);
}

static void test_inflix_expression()
{
    char *input = " \n\
        5 + 5; \n\
        6 - 5; \n\
        4 == 3; \n\
        ";

    int i;
    lexer l = lexer_new(input);
    statement *sts = ast_parse(&l, &i);

    print_sts(sts, i);

    assert(i == 3);
    assert(sts[0].token.token == T_INT);

    exp_inflix *exp = sts[0].exp.exp;
    exp_integer *left = exp->left.exp;
    exp_integer *right = exp->right.exp;

    assert(strcmp(exp->op, "+") == 0);
    assert(left->value == 5);
    assert(right->value == 5);

    exp = sts[1].exp.exp;
    left = exp->left.exp;
    right = exp->right.exp;
    assert(strcmp(exp->op, "-") == 0);
    assert(left->value == 6);
    assert(right->value == 5);

    exp = sts[2].exp.exp;
    left = exp->left.exp;
    right = exp->right.exp;
    assert(strcmp(exp->op, "==") == 0);
    assert(left->value == 4);
    assert(right->value == 3);

    free(sts);
}

static void test_inflix_expression_strings()
{
    typedef struct
    {
        char *one;
        char *two;
    } test_t;

    int n = 8;
    test_t testcases[] = {
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "a * b * c",
            "((a * b) * c)",
        },
        {
            "a * b / c",
            "((a * b) / c)",
        },
        {
            "a + b / c",
            "(a + (b / c))",
        },
        {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)",
        },
        {
            "3 + 4; -5 * 5",
            "(3 + 4)((-5) * 5)",
        },
        {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))",
        },
        {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))",
        },
    };

    bool r;
    str *buf = str_new("");

    for (int j = 0; j < n; j++) {
        int i = 0;
        lexer l = lexer_new(testcases[j].one);
        statement *sts = ast_parse(&l, &i);

        ast_to_str(buf, sts, i);
        r = str_cmp_char(buf, testcases[j].two);
        if (!r) {
            printf("exp: %s\n", testcases[j].two);
            printf("got: %s\n", buf->data);
            assert(r);
        }

        free(sts);
        str_reset(buf);
    }
}

int main(int argc, char *argv[])
{
    // test_lexer_read_char();
    // test_lexer_next_token();
    // test_lexer_more_operators();
    // test_if_else_return();
    // test_eq_not_eq();
    // test_ast_let_statement();
    // test_ast_return_statement();
    // test_identifier_expression();
    // test_integer_expression();
    // test_prefix_expression();
    // test_inflix_expression();
    test_inflix_expression_strings();

    printf("all tests passed\r\n");
}
