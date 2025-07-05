#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "asset_t.h"
#include "color_t.h"
#include "log.h"
#include "stdfunc.h"

#define BOXES_FILE_EXTENSION "boxes"

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

static FORCE_INLINE SDL_FRect** boxes_t$currentKeyFrames$get(
    const boxes_t* _boxes ) {
    SDL_FRect** l_returnValue = NULL;

    if ( UNLIKELY( !_boxes ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = createArray( SDL_FRect* );

        if ( UNLIKELY( _boxes->currentFrame >=
                       arrayLength( _boxes->frames ) ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Invalid boxex current frame" );

            goto EXIT;
        }

        const size_t* l_boxesIndexes =
            ( _boxes->frames[ _boxes->currentFrame ] );

        FOR_ARRAY( const size_t*, l_boxesIndexes ) {
            insertIntoArray( &l_returnValue,
                             ( _boxes->keyFrames[ *_element ] ) );
        }
    }

EXIT:
    return ( l_returnValue );
}

boxes_t boxes_t$create( void );
bool boxes_t$destroy( boxes_t* restrict _boxes );

bool boxes_t$load$one( boxes_t* restrict _boxes,
                       const SDL_FRect* restrict _targetRectangle,
                       size_t _startIndex,
                       size_t _endIndex );

// X Y Width Height StartIndex-EndIndex
bool boxes_t$load$one$fromString( boxes_t* restrict _boxes,
                                  char* restrict _string );
bool boxes_t$load$fromAsset( boxes_t* restrict _boxes,
                             const asset_t* restrict _asset );
bool boxes_t$load$fromPaths( boxes_t* restrict _boxes,
                             char* const* restrict _paths );
bool boxes_t$load$fromGlob( boxes_t* restrict _boxes,
                            const char* restrict _glob );
bool boxes_t$unload( boxes_t* restrict _boxes );

bool boxes_t$step( boxes_t* restrict _boxes, bool _canLoop );
bool boxes_t$render( const boxes_t* restrict _boxes,
                     SDL_Renderer* _renderer,
                     const SDL_FRect* restrict _targetRectanble,
                     bool _doFill );
