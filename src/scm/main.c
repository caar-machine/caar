#include <codegen.h>
#include <lib/log.h>
#include <readline/readline.h>
#include <string.h>

int main()
{
    while (1)
    {
        char *input = readline("\033[1;32mSCM-USER\033[0m > ");

        scm_compile(input, stdout);
    }

    return 0;
}