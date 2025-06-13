#pragma once

#include <SDL3/SDL_render.h>
#include <stdbool.h>

#include "state_t.h"

#define DEFAULT_OBJECT      \
    { .states = NULL,       \
      .currentState = NULL, \
      .worldX = 0,          \
      .worldY = 0,          \
      .worldXMin = 0,       \
      .worldYMin = 0,       \
      .worldXMax = 0,       \
      .worldYMax = 0 }

typedef struct {
    state_t** states;
    state_t* currentState;
    float worldX;
    float worldY;
    float worldXMin;
    float worldYMin;
    float worldXMax;
    float worldYMax;
} object_t;

object_t object_t$create( void );
bool object_t$destroy( object_t* restrict _object );

// First file - boxes
// fileName_ColorAsHex.extension
// X Y Width Height StartIndex-EndIndex
// After - animation
// fileName_WidthxHeight_StartIndex-EndIndex.extension
bool object_t$state$add$fromPaths( object_t* restrict _object,
                                   SDL_Renderer* _renderer,
                                   char* restrict _boxes,
                                   char* const* restrict _animation,
                                   bool _isActionable,
                                   bool _canLoop );
bool object_t$state$remove( object_t* restrict _object,
                            state_t* restrict _state );
bool object_t$states$remove( object_t* restrict _object );

bool object_t$move( object_t* restrict _object,
                    const float _x,
                    const float _y );
bool object_t$step( object_t* restrict _object,
                    float _velocityX,
                    float _velocityY );
bool object_t$render( const object_t* restrict _object,
                      const SDL_FRect* restrict _cameraRectangle,
                      bool _doDrawBoxes );
