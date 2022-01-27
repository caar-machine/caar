#include <as.h>
#include <ast.h>
#include <lex.h>
#include <stdio.h>

int get_file_size(FILE *fp)
{
    fseek(fp, 0, SEEK_END);

    int size = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    return size;
}

void as_assemble(char *input)
{
    FILE *fp = fopen(input, "r");

    int size = get_file_size(fp);

    char *text = malloc(size);

    fread(text, 1, size, fp);

    Tokens tokens = lex(text);

    parse(tokens);
}