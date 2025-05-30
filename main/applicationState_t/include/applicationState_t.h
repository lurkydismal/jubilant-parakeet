#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#include "camera_t.h"
#include "object_t.h"
#include "settings_t.h"

#define DEFAULT_APPLICATION_STATE_BACKGROUND DEFAULT_OBJECT

#define DEFAULT_APPLICATION_STATE                         \
    { .window = NULL,                                     \
      .renderer = NULL,                                   \
      .settings = DEFAULT_SETTINGS,                       \
      .totalFramesRendered = 0,                           \
      .background = DEFAULT_APPLICATION_STATE_BACKGROUND, \
      .camera = DEFAULT_CAMERA,                           \
      .logicalWidth = 1280,                               \
      .logicalHeight = 720,                               \
      .localPlayer = DEFAULT_PLAYER,                      \
      .remotePlayers = NULL }

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    settings_t settings;
    size_t totalFramesRendered;
    object_t background;
    camera_t camera;
    size_t logicalWidth;
    size_t logicalHeight;
    player_t localPlayer;
    player_t** remotePlayers;
} applicationState_t;

applicationState_t applicationState_t$create( void );
bool applicationState_t$destroy(
    applicationState_t* restrict _applicationState );
