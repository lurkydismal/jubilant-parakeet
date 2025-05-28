#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "asset_t.h"
#include "color_t.h"

#define DEFAULT_BOXES    \
    { .keyFrames = NULL, \
      .frames = NULL,    \
      .currentFrame = 0, \
      .color = DEFAULT_COLOR }

typedef struct {
    SDL_FRect** keyFrames;
    size_t** frames;
    size_t currentFrame;
    color_t color;
} boxes_t;

boxes_t boxes_t$create( void );
bool boxes_t$destroy( boxes_t* restrict _boxes );

bool boxes_t$load$one( boxes_t* restrict _boxes,
                       const SDL_FRect* restrict _targetRectangle,
                       size_t _startIndex,
                       size_t _endIndex );

// X Y Width Height StartIndex-EndIndex
bool boxes_t$load$one$fromString( boxes_t* restrict _boxes,
                                  const char* restrict _string );
bool boxes_t$load$fromAsset( boxes_t* restrict _boxes,
                             const asset_t* restrict _asset );
bool boxes_t$load$fromFiles( boxes_t* restrict _boxes,
                             char* const* restrict _files );
bool boxes_t$unload( boxes_t* restrict _boxes );

bool boxes_t$step( boxes_t* restrict _boxes, bool _canLoop );
bool boxes_t$render( const boxes_t* restrict _boxes,
                     SDL_Renderer* _renderer,
                     const SDL_FRect* restrict _targetRectanble,
                     bool _doFill );
