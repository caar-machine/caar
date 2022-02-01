#include <cpu.h>
#include <dev/disk.h>
#include <errno.h>
#include <lib/log.h>
#include <stdbool.h>
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
        ram_write((uint32_t)i + 0x1000, MEM_BYTE, buf[i], ram);
    }
}

int get_file_size(FILE *fp)
{
    fseek(fp, 0, SEEK_END);

    int size = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    return size;
}

void read_file(char *file, uint8_t *buffer, bool alloc)
{
    FILE *fp = fopen(file, "rb");

    if (fp == NULL)
    {
        perror("fopen(): couldn't open file");
        exit(errno);
    }

    int size = get_file_size(fp);

    if (alloc)
        buffer = malloc(size);

    fread(buffer, 1, size, fp);

    fclose(fp);
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

    read_file(file, buffer, false);

    write_to_ram(buffer, size, ram);

    fclose(fp);

    return size;
}

uint32_t ram_size = 16;

uint32_t get_memory_size()
{
    return ram_size * 1024 * 1024;
}

uint8_t *disk_data;

uint8_t *get_disk_data()
{
    return disk_data;
}

int main(int argc, char **argv)
{
    shift(&argc, &argv);

    Ram ram = {0};
    Cpu cpu = {0};
    Bus bus = {0};

    char *file = NULL;
    char *disk_file = NULL;

    while (argc > 0)
    {
        char *arg = shift(&argc, &argv);

        if (strcmp(arg, "-rom") == 0)
        {
            file = shift(&argc, &argv);
        }

        else if (strcmp(arg, "-disk") == 0)
        {
            disk_file = shift(&argc, &argv);
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

    if (!disk_file)
    {
        error("No disk file specified, try passing -disk to emulator");
        exit(-1);
    }

    disk_data = calloc(1024, 1);

    read_file(disk_file, disk_data, false);

    ram_init(&ram);
    bus_init(&bus);

    int size = load_file(file, &ram);

    disk_init(&bus);

    cpu_init(&ram, &bus, &cpu, size);

    while (1)
    {
        cpu_do_cycle(&cpu);
    }

    free(ram.buffer);

    return 0;
}
