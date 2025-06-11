#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#include "HUD_t.h"
#include "background_t.h"
#include "camera_t.h"
#include "config_t.h"
#include "player_t.h"
#include "settings_t.h"

#define DEFAULT_APPLICATION_STATE    \
    { .window = NULL,                \
      .renderer = NULL,              \
      .settings = DEFAULT_SETTINGS,  \
      .config = DEFAULT_CONFIG,      \
      .background = NULL,            \
      .HUD = NULL,                   \
      .camera = DEFAULT_CAMERA,      \
      .localPlayer = DEFAULT_PLAYER, \
      .remotePlayers = NULL,         \
      .logicalWidth = 1280,          \
      .logicalHeight = 720,          \
      .totalFramesRendered = 0 }

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    settings_t settings;
    config_t config;
    background_t* background;
    HUD_t* HUD;
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

bool applicationState_t$load( applicationState_t* _applicationState );
bool applicationState_t$unload( applicationState_t* _applicationState );
