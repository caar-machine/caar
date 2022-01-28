#include <cpu.h>
#include <errno.h>
#include <lib/log.h>
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

uint32_t ram_size = 16;

uint32_t get_memory_size()
{
    return ram_size * 1024 * 1024;
}

int main(int argc, char **argv)
{
    shift(&argc, &argv);

    Ram ram = {0};
    Cpu cpu = {0};
    char *file = NULL;

    while (argc > 0)
    {
        char *arg = shift(&argc, &argv);

        if (strcmp(arg, "-rom") == 0)
        {
            file = shift(&argc, &argv);
        }

        else if (strcmp(arg, "-m") == 0)
        {
            sscanf(shift(&argc, &argv), "%d", (int *)&ram_size);
        }

        else
        {
            printf("Unknown option: %s\n", arg);
        }
    }

    info("starting CAAR..");

    if ((MEMORY_SIZE / 1024 / 1024) < 8)
    {
        warn("memory size under 8mb is not recommended, the system might not work");
    }

    else
    {
        info("ram size is %d mb", MEMORY_SIZE / 1024 / 1024);
    }

    if (!file)
    {
        error("no bootrom specified, try passing -rom to the emulator");
        exit(-1);
    }

    ram_init(&ram);

    int size = load_file(file, &ram);

    cpu_init(&ram, &cpu, size);

    while (1)
    {
        cpu_do_cycle(&cpu);
    }

    free(ram.buffer);

    return 0;
}
