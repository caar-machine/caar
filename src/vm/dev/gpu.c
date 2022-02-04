#include <SDL2/SDL_render.h>
#include <dev/gpu.h>

Gpu current_gpu = {0};
bool active = false;

int fps = 60;
int frame_delay = 16;
int frame_time = 0;

uint32_t gpu_read()
{
    return FB_ADDRESS;
}

bool gpu_active()
{
    return active;
}

void gpu_draw(uint32_t pixel, uint32_t addr)
{
    current_gpu.pixels[addr - FB_ADDRESS] = pixel;
}

void gpu_write(uint32_t data, Ram *ram)
{
    (void)data;
    (void)ram;

    warn("Trying to write to gpu");
}

void gpu_update()
{

    SDL_UpdateTexture(current_gpu.texture, NULL, current_gpu.pixels, current_gpu.width * sizeof(uint32_t));
    SDL_RenderClear(current_gpu.renderer);
    SDL_RenderCopy(current_gpu.renderer, current_gpu.texture, NULL, NULL);
    SDL_RenderPresent(current_gpu.renderer);
}

void gpu_init(int width, int height, Bus *bus)
{
    SDL_Init(SDL_INIT_VIDEO);

    current_gpu.window = SDL_CreateWindow("CAAR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                          SDL_WINDOW_SHOWN);
    current_gpu.width = width;
    current_gpu.height = height;

    current_gpu.renderer = SDL_CreateRenderer(current_gpu.window, -1,
                                              SDL_RENDERER_ACCELERATED);
    current_gpu.pixels = calloc(width * height, 1);

    current_gpu.texture = SDL_CreateTexture(current_gpu.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

    BusDevice dev = {.read = gpu_read, .write = gpu_write, .type = BUS_DEV_GPU};

    bus_attach(dev, bus);
}