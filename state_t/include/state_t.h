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

bool state_t$load$fromPaths( state_t* restrict _state,
                             char* restrict _boxesPath,
                             char* const* restrict _animationPath );
bool state_t$load$fromGlob( state_t* restrict _state,
                            const char* restrict _boxesGlob,
                            const char* restrict _animationGlob );
bool state_t$unload( state_t* restrict _state );

bool state_t$step( state_t* restrict _state );
bool state_t$render$rotated( const state_t* restrict _state,
                             const double _angle,
                             SDL_FlipMode _flipMode,
                             const SDL_FRect* restrict _targetRectanble,
                             bool _doDrawBoxes );
bool state_t$render( const state_t* restrict _state,
                     const SDL_FRect* restrict _targetRectanble,
                     bool _doDrawBoxes );
