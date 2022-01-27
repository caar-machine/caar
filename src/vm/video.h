#ifndef CAAR_VIDEO_H
#define CAAR_VIDEO_H
#include <SDL2/SDL.h>
#include <ram.h>

#define WIDTH 1024
#define HEIGHT 768
#define FB_SIZE (WIDTH * sizeof(uint32_t)) * HEIGHT

typedef struct
{
    SDL_Window *window;
    int width, height;
    SDL_Renderer *renderer;
    uint32_t *pixels;
    SDL_Texture *fb;
} Video;

void video_init(int width, int height, Video *video);
void video_update(Ram *ram, Video *video);

#endif