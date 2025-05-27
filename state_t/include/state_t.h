#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#include "animation_t.h"
#include "boxes_t.h"

#define DEFAULT_STATE                 \
    { .animation = DEFAULT_ANIMATION, \
      .boxes = DEFAULT_BOXES,         \
      .isActionable = true,           \
      .canLoop = false,               \
      .renderer = NULL }

typedef struct {
    animation_t animation;
    boxes_t boxes;
    bool isActionable;
    bool canLoop;
    SDL_Renderer* renderer;
} state_t;

state_t state_t$create( void );
bool state_t$destroy( state_t* restrict _state );

bool state_t$load$fromFiles( state_t* restrict _state,
                             char* const* restrict _files );
bool state_t$unload( state_t* restrict _state );

bool state_t$step( state_t* restrict _state );
bool state_t$render( const state_t* restrict _state,
                     const SDL_FRect* restrict _targetRectanble,
                     bool _doDrawBoxes );
