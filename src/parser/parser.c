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
    [T_MINUS] = "-",
    [T_BANG] = "!",
    [T_ASTERISK] = "*",
    [T_SLASH] = "/",
    [T_LT] = "<",
    [T_GT] = ">",
    [T_COMMA] = ",",
    [T_SEMICOLON] = ";",
    [T_LPAREN] = "(",
    [T_RPAREN] = ")",
    [T_LBRACE] = "{",
    [T_RBRACE] = "}",
    [T_FUNCTION] = "FUNCTION",
    [T_LET] = "LET",
    [T_TRUE] = "TRUE",
    [T_FALSE] = "FALSE",
    [T_IF] = "IF",
    [T_ELSE] = "ELSE",
    [T_RETURN] = "RETURN",
    [T_EQ] = "EQUAL",
    [T_NOT_EQ] = "NOT_EQUAL",
};

static const char *const KEYWORDS[] = {
    [T_LET] = "let",
    [T_FUNCTION] = "fn",
    [T_TRUE] = "true",
    [T_FALSE] = "false",
    [T_IF] = "if",
    [T_ELSE] = "else",
    [T_RETURN] = "return",
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

static unsigned char lexer_peek_next(lexer *l)
{
    return l->input[l->readPos];
}

static bool next_char_is(lexer *l, const char *c)
{
    char buf[2] = "";
    sprintf(buf, "%c", l->input[l->readPos]);
    return strcmp(c, buf) == 0;
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
        .literal = "",
    };
    lexer_read_char(l);

    if (l->ch == 0)
        return t;

    if (is_whitespace(l->ch))
        return lexer_next_token(l);

    char ch_buf[2] = "";
    sprintf(ch_buf, "%c", l->ch);

    // operators / characters
    for (int i = 0; i < T_MAX_TOKENS; i++) {
        if (strcmp(TOKENS[i], ch_buf) == 0) {
            // ==
            if (strcmp(ch_buf, "=") == 0 && next_char_is(l, "=")) {
                lexer_read_char(l);
                return (token){
                    .token = T_EQ,
                    .literal = "==",
                };
            }
            // !=
            if (strcmp(ch_buf, "!") == 0 && next_char_is(l, "=")) {
                lexer_read_char(l);
                return (token){
                    .token = T_NOT_EQ,
                    .literal = "!=",
                };
            }

            return (token){
                .token = i,
                .literal = strdup(ch_buf),
            };
        }
    }

    // read all till whitespace or EOF;
    char literal_buf[64] = "";

    // strings
    if (is_letter(l->ch)) {
        while (!l->eof) {
            sprintf(ch_buf, "%c", l->ch);
            strcat(literal_buf, ch_buf);

            if (!lexer_peek_next_char_is(l, &is_letter))
                break;

            lexer_read_char(l);
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
