#include <as.h>
#include <lib/log.h>
#include <string.h>

char *shift(int *argc, char ***argv)
{
    char *arg = *(*argv)++;

    (*argc)--;

    return arg;
}

int main(int argc, char **argv)
{
    char *input = NULL;
    char *output = NULL;

    while (argc > 0)
    {
        char *arg = shift(&argc, &argv);

        if (strcmp(arg, "-o") == 0)
        {
            output = shift(&argc, &argv);
        }

        else
        {
            input = arg;
        }
    }

    if (!input)
    {
        error("no input file specified");
    }

    if (!output)
    {
        char *str = "output.caar";
        output = str;
    }

    as_assemble(input, output);

    return 0;
}
