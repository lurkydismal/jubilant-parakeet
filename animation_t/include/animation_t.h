#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "boxes_t.h"
#include "cpp_compatibility.h"

#define DEFAULT_ANIMATION \
    { .keyFrames = NULL,  \
      .frames = NULL,     \
      .currentFrame = 0,  \
      .targetBoxes = DEFAULT_BOXES }

typedef struct {
    SDL_Texture** keyFrames;
    size_t* frames;
    size_t currentFrame;
    boxes_t targetBoxes;
} animation_t;

animation_t animation_t$create( void );
bool animation_t$destroy( animation_t* restrict _animation );

bool animation_t$load$fromAsset( animation_t* restrict _animation,
                                 SDL_Renderer* _renderer,
                                 const asset_t* restrict _asset,
                                 const SDL_FRect* restrict _targetRectangle,
                                 size_t _startIndex,
                                 size_t _endIndex );

// fileName_WidthxHeight_StartIndex-EndIndex.extension
bool animation_t$load$fromPaths( animation_t* restrict _animation,
                                 SDL_Renderer* _renderer,
                                 char* const* restrict _paths );
bool animation_t$load$fromGlob( animation_t* restrict _animation,
                                SDL_Renderer* _renderer,
                                const char* restrict _glob );
bool animation_t$unload( animation_t* restrict _animation );

bool animation_t$step( animation_t* restrict _animation, bool _canLoop );
bool animation_t$render( const animation_t* restrict _animation,
                         SDL_Renderer* _renderer,
                         const SDL_FRect* restrict _targetRectangle );
