#include <as.h>
#include <ast.h>
#include <codegen.h>
#include <lex.h>
#include <stdio.h>

int get_file_size(FILE *fp)
{
    fseek(fp, 0, SEEK_END);

    int size = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    return size;
}

void as_assemble(char *input, char *output)
{
    FILE *fp = fopen(input, "r");

    int size = get_file_size(fp);

    char *text = malloc(size);

    fread(text, 1, size, fp);

    fclose(fp);

    Tokens tokens = lex(text);

    free(text);

    Ast ast = parse(tokens);

    Bytes bytes = codegen(ast);

    FILE *output_f = fopen(output, "wb");

    for (int i = 0; i < bytes.length; i++)
    {
        fputc(bytes.data[i], output_f);
    }

    fclose(output_f);

    vec_deinit(&bytes);
    vec_deinit(&ast);
    vec_deinit(&tokens);
}