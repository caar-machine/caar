#include <as.h>
#include <stdio.h>

int main()
{
    as_assemble("src/bios/main.lisp");

    printf("Hello World!\n");
    return 0;
}
