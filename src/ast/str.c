#include "ast/str.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

str *str_new(char *initial_value)
{
    str *s = malloc(sizeof(str));
    s->cap = 128;
    s->len = 0;
    s->data = calloc(sizeof(char), 128);

    if (initial_value != NULL && strlen(initial_value))
        str_appendf(s, "%s", initial_value);

    return s;
}

void str_appendf(str *s, const char *format, ...)
{
    // TODO: potential bug here if resulting string >256 characters.
    char buf[256];

    va_list args;
    va_start(args, format);
    int len = vsprintf(buf, format, args);
    va_end(args);

    // TODO: check if len <= 128
    if (s->len + len + 1 > s->cap) {
        // reaclloc
        size_t ncap = s->cap + 128;
        char *data = realloc(s->data, ncap);

        if (!data) {
            printf("failed to allocate mem for str");
            abort();
        }

        s->data = data;
        s->cap = ncap;
    }

    s->len += len;

    strncat(s->data, buf, len);
}

void str_reset(str *s)
{
    memset(s->data, 0, s->len);
    s->len = 0;
}

void str_free(str *s)
{
    free(s->data);
    free(s);
}

bool str_cmp(str *s1, str *s2)
{
    return strcmp(s1->data, s2->data) == 0;
}

bool str_cmp_char(str *s1, char *s2)
{
    return strcmp(s1->data, s2) == 0;
}
