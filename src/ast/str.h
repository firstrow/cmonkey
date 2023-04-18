#ifndef STR_H
#define STR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    size_t cap;
    size_t len;
    char *data;
} str;

str *str_new(char *initial_value);
void str_appendf(str *s, const char *format, ...);
void str_reset(str *s);
void str_free(str *s);
bool str_cmp(str *s1, str *s2);
bool str_cmp_char(str *s1, char *s2);

#endif // STR_H
