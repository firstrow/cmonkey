#ifndef PARSER_H
#define PARSER_H

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
    // delimiters
    T_COMMA,
    T_SEMICOLON,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_FUNCTION,
    T_LET,
} token_type;

typedef struct {
    char *input;
    int len;
    int pos;
    int readPos;
    unsigned char ch;
} lexer;

extern const char * const tokens[];

lexer parser_new(char *str);
void lexer_read_char(lexer *l);

#endif // PARSER_H
