#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#include "camera_t.h"
#include "object_t.h"
#include "settings_t.h"
#include "background_t.h"

#define DEFAULT_APPLICATION_STATE                         \
    { .window = NULL,                                     \
      .renderer = NULL,                                   \
      .settings = DEFAULT_SETTINGS,                       \
      .totalFramesRendered = 0,                           \
      .background = DEFAULT_BACKGROUND, \
      .camera = DEFAULT_CAMERA,                           \
      .logicalWidth = 1280,                               \
      .logicalHeight = 720,                               \
      .localPlayer = DEFAULT_PLAYER,                      \
      .remotePlayers = NULL }

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    settings_t settings;
    background_t* background;
    camera_t camera;
    player_t localPlayer;
    player_t** remotePlayers;
    size_t logicalWidth;
    size_t logicalHeight;
    size_t totalFramesRendered;
} applicationState_t;

applicationState_t applicationState_t$create( void );
bool applicationState_t$destroy(
    applicationState_t* restrict _applicationState );
