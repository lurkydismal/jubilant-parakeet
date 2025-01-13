#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "color_t.h"

#define DEFAULT_BOXES    \
    { .keyFrames = NULL, \
      .frames = NULL,    \
      .currentFrame = 1, \
      .color = DEFAULT_COLOR }

typedef struct {
    SDL_FRect** keyFrames;
    size_t** frames;
    size_t currentFrame;
    color_t color;
} boxes_t;

boxes_t boxes_t$create( void );
void boxes_t$destroy( boxes_t* _boxes );

boxes_t boxes_t$load$fromString( const char* _string,
                                 const char* _boxesColorAsString );
boxes_t boxes_t$load( const char* _path, const char* _pattern );
void boxes_t$unload( boxes_t* _boxes );

void boxes_t$step( boxes_t* _boxes, bool _canLoop );
void boxes_t$render( SDL_Renderer* _renderer,
                     const boxes_t* _boxes,
                     const SDL_FRect* _targetRectanble,
                     bool _doFill );
