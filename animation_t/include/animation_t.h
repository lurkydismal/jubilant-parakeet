#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

#include "boxes_t.h"

#define DEFAULT_ANIMATION \
    { .keyFrames = NULL,  \
      .frames = NULL,     \
      .currentFrame = 1,  \
      .targetBoxes = DEFAULT_BOXES }

typedef struct {
    SDL_Texture** keyFrames;
    size_t* frames;
    size_t currentFrame;
    boxes_t targetBoxes; // TODO: Change target to a better name
} animation_t;

animation_t animation_t$create( void );
void animation_t$destroy( animation_t* _animation );

animation_t animation_t$load( SDL_Renderer* _renderer,
                              const char* _path,
                              const char* _pattern );
void animation_t$unload( animation_t* _animation );

void animation_t$step( animation_t* _animation, bool _canLoop );
void animation_t$render( SDL_Renderer* _renderer,
                         const animation_t* _animation,
                         const SDL_FRect* _targetRectangle );
