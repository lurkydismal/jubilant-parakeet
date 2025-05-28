#pragma once

#include <SDL3/SDL_render.h>
#include <stdbool.h>

#include "state_t.h"

#define DEFAULT_OBJECT   \
    { .states = NULL,    \
      .currentState = 0, \
      .worldX = 0,       \
      .worldY = 0,       \
      .worldXMin = 0,    \
      .worldYMin = 0,    \
      .worldXMax = 0,    \
      .worldYMax = 0 }

typedef struct {
    state_t** states;
    size_t currentState;
    float worldX;
    float worldY;
    float worldXMin;
    float worldYMin;
    float worldXMax;
    float worldYMax;
} object_t;

object_t object_t$create( void );
bool object_t$destroy( object_t* restrict _object );

bool object_t$add$state$fromFiles( object_t* restrict _object,
                                   SDL_Renderer* _renderer,
                                   char* const* restrict _files,
                                   bool _isActionable,
                                   bool _canLoop );

bool object_t$move( object_t* restrict _object, float _x, float _y );
bool object_t$step( object_t* restrict _object,
                    float _velocityX,
                    float _velocityY );
bool object_t$render( const object_t* restrict _object,
                      const object_t* restrict _camera,
                      bool _doDrawBoxes );
