#pragma once

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include "animation_t.h"
#include "boxes_t.h"

#define DEFAULT_STATE                 \
    { .renderer = NULL,               \
      .animation = DEFAULT_ANIMATION, \
      .boxes = DEFAULT_BOXES,         \
      .isActionable = true }

typedef struct {
    SDL_Renderer* renderer;
    animation_t animation;
    boxes_t boxes;
    bool isActionable;
} state_t;

state_t state_t$create( SDL_Renderer* _renderer,
                        const char* _path,
                        const char* _name,
                        bool _isActionable );
void state_t$destroy( state_t* _state );
void state_t$step( state_t* _state, bool _canLoop );
void state_t$render( state_t* _state,
                     const SDL_FRect* _targetRectanble,
                     bool _doDrawBoxes );
