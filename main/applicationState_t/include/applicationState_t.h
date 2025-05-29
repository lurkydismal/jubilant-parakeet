#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#include "camera_t.h"
#include "settings_t.h"

#define DEFAULT_APPLICATION_STATE   \
    { .window = NULL,               \
      .renderer = NULL,             \
      .settings = DEFAULT_SETTINGS, \
      .totalFramesRendered = 0,     \
      .logicalWidth = 1280,         \
      .logicalHeight = 720 }

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    settings_t settings;
    size_t totalFramesRendered;
    camera_t camera;
    // TODO: Refactor
    size_t logicalWidth;
    size_t logicalHeight;
} applicationState_t;

applicationState_t applicationState_t$create( void );
bool applicationState_t$destroy(
    applicationState_t* restrict _applicationState );
