#pragma once

#include <SDL3/SDL_render.h>
#include <stddef.h>
#include <stdbool.h>

#define DEFAULT_TEXTURE {\
    .data = NULL, \
    .height = 0,\
    .width =0, \
}

typedef struct {
    SDL_Texture* data;
    size_t height;
    size_t width;
} texture_t;
