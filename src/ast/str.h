#ifndef STR_H
#define STR_H

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

#endif // STR_H
