#include "lexer/lexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <readline/history.h>
#include <readline/readline.h>

int main(int argc, char *argv[])
{
    char *input;
    while (true) {
        input = readline("> ");
        if (input == NULL)
            continue;

        add_history(input);
        printf("got: %s\r\n", input);
    }
}
