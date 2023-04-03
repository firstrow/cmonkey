#include "parser/parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_lexer()
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

int main(int argc, char *argv[])
{
    test_lexer();
    printf("all tests passed\r\n");
}
