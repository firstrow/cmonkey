#include "lexer/lexer.h"
#include <assert.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/ast.h"

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

    assert(i == 3);
    assert(sts[0].token.token == T_LET);
    assert(strcmp(sts[0].literal, "x") == 0);
    assert(sts[1].token.token == T_LET);
    assert(strcmp(sts[1].literal, "y") == 0);
    assert(sts[2].token.token == T_LET);
    assert(strcmp(sts[2].literal, "foobar") == 0);

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

    free(sts);
}

int main(int argc, char *argv[])
{
    test_lexer_read_char();
    test_lexer_next_token();
    test_lexer_more_operators();
    test_if_else_return();
    test_eq_not_eq();
    test_ast_let_statement();
    test_ast_return_statement();
    test_identifier_expression();

    printf("all tests passed\r\n");
}
