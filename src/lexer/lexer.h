#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

typedef enum
{
    T_ILLEGAL = 0,
    T_EOF,
    // identifiers + literals
    T_INDENT,
    T_INT,
    // operators
    T_ASSIGN,
    T_PLUS,
    T_MINUS,
    T_BANG,
    T_ASTERISK,
    T_SLASH,
    T_LT,
    T_GT,
    T_EQ,
    T_NOT_EQ,
    // delimiters
    T_COMMA,
    T_SEMICOLON,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    // keywords
    T_FUNCTION,
    T_LET,
    T_TRUE,
    T_FALSE,
    T_IF,
    T_ELSE,
    T_RETURN,

    T_MAX_TOKENS
} token_t;

typedef struct {
    token_t token;
    char *literal;
} token;

typedef struct {
    char *input;
    int len;
    int pos;
    int readPos;
    unsigned char ch;
    bool eof;
} lexer;

extern const char * const TOKENS[];

lexer lexer_new(char *str);
void lexer_read_char(lexer *l);
token lexer_next_token(lexer *l);

#endif // PARSER_H
