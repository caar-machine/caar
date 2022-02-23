#include <dev/gpu.h>
#include <lib/cglm/cglm.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

Gpu current_gpu = {0};
bool active = false;

int fps = 60;
int frame_delay = 16;
int frame_time = 0;

float gpu_fb_vertices[] = {
    0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
    1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,  0.0f, 0.0f
};
unsigned int gpu_fb_indices[] = {
    0, 1, 2, 2, 3, 0
};

GLuint gpu_shaderprog;

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
    glClearColor(0, 0.5, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Gpu_obj *obj = &current_gpu.object;
    glBindVertexArray(obj->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
    glBufferData(GL_ARRAY_BUFFER, obj->vsize, obj->vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->isize, obj->indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    mat4 model = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    glm_translate(model, (vec3){obj->pos.x, obj->pos.y, 0});
    glm_scale(model, (vec3){obj->scale.x, obj->scale.y, 1});
    int uHandle = glGetUniformLocation(gpu_shaderprog, "m");
    glUniformMatrix4fv(uHandle, 1, GL_FALSE, *model);
    glBindTexture(GL_TEXTURE_2D, obj->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, current_gpu.width, current_gpu.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, current_gpu.pixels);
    glUniform1i(glGetUniformLocation(gpu_shaderprog, "d"), 0);
    glDrawElements(GL_TRIANGLES, obj->isize / sizeof(*obj->indices), GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glfwPollEvents();
    if (glfwWindowShouldClose(current_gpu.window)) {
        SDL_Event event;
        event.type = SDL_QUIT;
        SDL_PushEvent(&event);
    }
    glfwSwapInterval(1);
    glfwSwapBuffers(current_gpu.window);
}

bool gpu_initshaders(char* vstext, char* fstext, GLuint* p) {
    GLuint vertexHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexHandle, 1, (const GLchar * const*)&vstext, NULL);
    puts("OK");
    glCompileShader(vertexHandle);
    GLint ret = GL_FALSE;
    glGetShaderiv(vertexHandle, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        GLint logSize = 0;
        glGetShaderiv(vertexHandle, GL_INFO_LOG_LENGTH, &logSize);
        GLchar* log = malloc((logSize + 1) * sizeof(GLchar));
        glGetShaderInfoLog(vertexHandle, logSize, &logSize, log);
        log[logSize - 1] = 0;
        fprintf(stderr, "Vertex shader compile error: %s", (char*)log);
        free(log);
        glDeleteShader(vertexHandle);
        return false;
    }
    GLuint fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragHandle, 1, (const GLchar * const*)&fstext, NULL);
    glCompileShader(fragHandle);
    glGetShaderiv(fragHandle, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        GLint logSize = 0;
        glGetShaderiv(fragHandle, GL_INFO_LOG_LENGTH, &logSize);
        GLchar* log = malloc((logSize + 1) * sizeof(GLchar));
        glGetShaderInfoLog(fragHandle, logSize, &logSize, log);
        log[logSize - 1] = 0;
        fprintf(stderr, "Fragment shader compile error: %s", (char*)log);
        free(log);
        glDeleteShader(vertexHandle);
        glDeleteShader(fragHandle);
        return false;
    }
    *p = glCreateProgram();
    glAttachShader(*p, vertexHandle);
    glAttachShader(*p, fragHandle);
    glLinkProgram(*p);
    glGetProgramiv(*p, GL_LINK_STATUS, &ret);
    glDeleteShader(vertexHandle);
    glDeleteShader(fragHandle);
    if (!ret) {
        GLint logSize = 0;
        glGetProgramiv(*p, GL_INFO_LOG_LENGTH, &logSize);
        GLchar* log = malloc((logSize + 1) * sizeof(GLchar));
        glGetProgramInfoLog(*p, logSize, &logSize, log);
        log[logSize - 1] = 0;
        fprintf(stderr, "Shader program link error: %s", (char*)log);
        free(log);
        return false;
    }
    return true;
}

void gpu_init(int width, int height, Bus *bus)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    current_gpu.window = glfwCreateWindow(width, height, "CAAR", NULL, NULL);
    current_gpu.width = width;
    current_gpu.height = height;

    glfwMakeContextCurrent(current_gpu.window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    gpu_initshaders(gpu_fb_vshader, gpu_fb_fshader, &gpu_shaderprog);
    glUseProgram(gpu_shaderprog);

    glViewport(0, 0, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(current_gpu.window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(current_gpu.window);

    current_gpu.pixels = calloc(width * height * sizeof(uint32_t), 1);

    Gpu_obj *obj = &current_gpu.object;
    obj->pos = (Gpu_coord){-1.0, -1.0};
    obj->scale = (Gpu_coord){2.0, 2.0};
    glGenVertexArrays(1, &obj->VAO);
    glGenBuffers(1, &obj->VBO);
    glGenBuffers(1, &obj->EBO);
    obj->vertices = gpu_fb_vertices;
    obj->vsize = sizeof(gpu_fb_vertices);
    obj->indices = gpu_fb_indices;
    obj->isize = sizeof(gpu_fb_indices);
    glGenTextures(1, &obj->texture);
    glBindTexture(GL_TEXTURE_2D, obj->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, current_gpu.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    BusDevice dev = {.read = gpu_read, .write = gpu_write, .type = BUS_DEV_GPU};

    bus_attach(dev, bus);
}
