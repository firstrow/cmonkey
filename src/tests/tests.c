#include "parser/parser.h"
#include <assert.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    lexer l = parser_new(input);

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
    lexer l = parser_new(input);

    token t = lexer_next_token(&l);
    assert(t.token == T_LET);

    t = lexer_next_token(&l);
    assert(t.token == T_INDENT);
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
    assert(t.token == T_INDENT);
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

void handler(int sig)
{
    void *array[10];
    size_t size;
    size = backtrace(array, 10);
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char *argv[])
{
    // TODO: finish;
    // signal(SIGSEGV, handler);

    test_lexer_read_char();
    test_lexer_next_token();

    printf("all tests passed\r\n");
}
