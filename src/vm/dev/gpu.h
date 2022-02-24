#ifndef CAAR_GPU_H
#define CAAR_GPU_H
#include <SDL2/SDL.h>
#include <cpu.h>
#include <dev/bus.h>
#include <stdbool.h>

#define FB_SIZE 0xc0000
#define FB_ADDRESS (uint32_t)(MEMORY_SIZE - STACK_SIZE - FB_SIZE)

typedef struct
{
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *texture;
    uint32_t *pixels;
    int width, height;
} Gpu;

void gpu_init(int width, int height, Bus *bus);
void gpu_draw(uint32_t pixel, uint32_t addr);

void gpu_update();

bool gpu_active();

#endif
