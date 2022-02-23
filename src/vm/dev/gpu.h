#ifndef CAAR_GPU_H
#define CAAR_GPU_H
#include <lib/glad/glad.h>
#include <GLFW/glfw3.h>
#include <cpu.h>
#include <dev/bus.h>
#include <stdbool.h>

#define FB_SIZE 0xc0000
#define FB_ADDRESS (uint32_t)(MEMORY_SIZE - STACK_SIZE - FB_SIZE)

extern char *gpu_fb_vshader, *gpu_fb_fshader;

typedef struct {
    float x;
    float y;
} Gpu_coord;

typedef struct
{
    Gpu_coord pos;
    Gpu_coord scale;
    float* vertices;
    long unsigned int vsize;
    unsigned int* indices;
    long unsigned int isize;
    GLuint VBO, VAO, EBO;
    GLuint texture;
} Gpu_obj;

typedef struct
{
    GLFWwindow *window;
    Gpu_obj object;
    uint32_t *pixels;
    int width, height;
} Gpu;

void gpu_init(int width, int height, Bus *bus);
void gpu_draw(uint32_t pixel, uint32_t addr);

void gpu_update();

bool gpu_active();

#endif
