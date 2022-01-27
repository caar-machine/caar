#include <cpu.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *shift(int *argc, char ***argv)
{
    char *arg = *(*argv)++;

    (*argc)--;

    return arg;
}

void write_to_ram(uint8_t *buf, size_t size, Ram *ram)
{
    for (size_t i = 0; i < size; i++)
    {
        ram_write((uint32_t)i + 0x1000, buf[i], ram);
    }
}

int get_file_size(FILE *fp)
{
    fseek(fp, 0, SEEK_END);

    int size = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    return size;
}

int load_file(char *file, Ram *ram)
{
    FILE *fp = fopen(file, "rb");

    if (fp == NULL)
    {
        perror("fopen(): couldn't open file");
        exit(errno);
    }

    int size = get_file_size(fp);

    uint8_t *buffer = malloc(size);

    fread(buffer, 1, size, fp);

    write_to_ram(buffer, size, ram);

    return size;
}

int main(int argc, char **argv)
{
    shift(&argc, &argv);

    Ram ram = {0};
    Cpu cpu = {0};
    char *file = NULL;

    ram_init(&ram);

    while (argc > 0)
    {
        char *arg = shift(&argc, &argv);

        if (strcmp(arg, "-rom") == 0)
        {
            file = shift(&argc, &argv);
        }

        else
        {
            printf("Unknown option: %s\n", arg);
        }
    }

    int size = load_file(file, &ram);

    cpu_init(&ram, &cpu, size);

    while (1)
    {
        cpu_do_cycle(&cpu);
    }

    free(ram.buffer);

    return 0;
}
