#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "boxes_t.h"
#include "log.h"
#include "stdfunc.h"

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

static FORCE_INLINE SDL_Texture* animation_t$currentKeyFrame$get(
    const animation_t* _animation ) {
    SDL_Texture* l_returnValue = NULL;

    if ( UNLIKELY( !_animation ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        if ( UNLIKELY( _animation->currentFrame >=
                       arrayLength( _animation->frames ) ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Invalid animation current frame" );

            goto EXIT;
        }

        l_returnValue =
            ( _animation->keyFrames
                  [ _animation->frames[ _animation->currentFrame ] ] );
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE SDL_FRect* animation_t$currentTargetRectangle$get(
    const animation_t* _animation ) {
    SDL_FRect* l_returnValue = NULL;

    if ( UNLIKELY( !_animation ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const boxes_t* l_animationTargetBox = &( _animation->targetBoxes );

        if ( UNLIKELY( l_animationTargetBox->currentFrame >=
                       arrayLength( l_animationTargetBox->frames ) ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Invalid target box current frame" );

            goto EXIT;
        }

        // Always a single box
        l_returnValue = l_animationTargetBox->keyFrames[ arrayFirstElement(
            l_animationTargetBox
                ->frames[ l_animationTargetBox->currentFrame ] ) ];
    }

EXIT:
    return ( l_returnValue );
}

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
bool animation_t$render$rotated( const animation_t* restrict _animation,
                                 SDL_Renderer* _renderer,
                                 const double _angle,
                                 SDL_FlipMode _flipMode,
                                 const SDL_FRect* restrict _targetRectangle );
bool animation_t$render( const animation_t* restrict _animation,
                         SDL_Renderer* _renderer,
                         const SDL_FRect* restrict _targetRectangle );
