#include "parser/parser.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef bool (*cb_checker)(unsigned char);

const char *const TOKENS[] = {
    [T_ILLEGAL] = "ILLEGAL",
    [T_EOF] = "EOF",
    [T_INDENT] = "INDENT",
    [T_INT] = "INT",
    [T_ASSIGN] = "=",
    [T_PLUS] = "+",
    [T_COMMA] = ",",
    [T_SEMICOLON] = ";",
    [T_LPAREN] = "(",
    [T_RPAREN] = ")",
    [T_LBRACE] = "{",
    [T_RBRACE] = "}",
    [T_FUNCTION] = "FUNCTION",
    [T_LET] = "LET",
};

static const char *const KEYWORDS[] = {
    [T_LET] = "let",
    [T_FUNCTION] = "fn",
};

lexer parser_new(char *str)
{
    lexer l = {
        .input = strdup(str),
        .len = strlen(str),
        .pos = 0,
        .readPos = 0,
        .ch = 0,
    };
    return l;
}

void lexer_read_char(lexer *l)
{
    if (l->readPos >= l->len) {
        l->ch = 0;
        l->eof = true;
        return;
    }

    l->ch = l->input[l->readPos];
    l->pos = l->readPos;
    l->readPos += 1;
}

// https://en.cppreference.com/w/c/language/ascii
static bool is_letter(unsigned char c)
{
    if (c > 64 && c < 91)
        return true;

    if (c > 96 && c < 123)
        return true;

    // _
    if (c == 95)
        return true;

    return false;
}

static bool is_number(unsigned char c)
{
    return c > 47 && c < 58;
}

static bool is_whitespace(unsigned char c)
{
    return isspace(c);
}

static bool lexer_peek_next_char_is(lexer *l, cb_checker cb)
{
    return cb(l->input[l->readPos]);
}

// read one char at a time till we reach whitespace;
// word search in tokens or keywords
token lexer_next_token(lexer *l)
{
    token t = {
        .token = T_EOF,
        .literal = NULL,
    };
    lexer_read_char(l);

    if (l->ch == 0)
        return t;

    if (is_whitespace(l->ch))
        return lexer_next_token(l);

    char ch_buf[2] = "";
    sprintf(ch_buf, "%c", l->ch);

    // check common cases first
    int checks[] = {T_ASSIGN, T_PLUS, T_COMMA, T_SEMICOLON, T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE};

    for (int i = 0; i <= 7; i++) {
        if (strcmp(TOKENS[checks[i]], ch_buf) == 0) {
            return (token){
                .token = checks[i],
                .literal = strdup(ch_buf),
            };
        }
    }

    // read all till whitespace or EOF;
    char literal_buf[64] = "";

    // stings
    if (is_letter(l->ch)) {
        while (!l->eof) {
            sprintf(ch_buf, "%c", l->ch);
            strcat(literal_buf, ch_buf);

            lexer_read_char(l);
            if (is_whitespace(l->ch) || l->ch == 0)
                break;
        }

        // keywords
        for (int i = 0; i < T_MAX_TOKENS; i++) {
            if (KEYWORDS[i] == NULL)
                continue;
            if (strcmp(KEYWORDS[i], literal_buf) == 0) {
                return (token){
                    .token = i,
                    .literal = strdup(literal_buf),
                };
            }
        }

        return (token){
            .token = T_INDENT,
            .literal = strdup(literal_buf),
        };
    }

    // numbers
    if (is_number(l->ch)) {
        while (!l->eof) {
            sprintf(ch_buf, "%c", l->ch);
            strcat(literal_buf, ch_buf);

            if (!lexer_peek_next_char_is(l, &is_number))
                break;

            lexer_read_char(l);
        }

        return (token){
            .token = T_INT,
            .literal = strdup(literal_buf),
        };
    }

    return (token){
        .token = T_ILLEGAL,
        .literal = strdup(literal_buf),
    };
}
