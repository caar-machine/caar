#include "ram.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cpu.h>
#include <video.h>

void video_init(int width, int height, Video *video)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    video->width = width;
    video->height = height;

    video->window = SDL_CreateWindow("CAAR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    video->renderer = SDL_CreateRenderer(video->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    video->fb = SDL_CreateTexture(video->renderer,
                                  SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

    video->pixels = calloc(width * height, sizeof(uint32_t));
}

uint32_t color_u32(int r, int g, int b)
{
    return (uint32_t)((255 << 24) + (r << 16) + (g << 8) + (b << 0));
}

void video_update(Ram *ram, Video *video)
{
    (void)ram;

    SDL_RenderClear(video->renderer);

    SDL_UpdateTexture(video->fb, NULL, video->pixels, video->width * sizeof(uint32_t));

    SDL_RenderCopy(video->renderer, video->fb, NULL, NULL);

    SDL_RenderPresent(video->renderer);
}