#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint64 totalFrameCount;
} applicationState_t;
