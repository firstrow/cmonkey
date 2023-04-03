#include "parser/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    parser_new("=+{}}[]");
    printf("%s", "done");
}
