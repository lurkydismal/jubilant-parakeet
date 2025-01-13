#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "state_t.h"

#define DEFAULT_OBJECT \
    { .states = NULL, .currentState = NULL, .worldX = 0, .worldY = 0 }

typedef struct {
    state_t** states;
    state_t* currentState;
    float worldX;
    float worldY;
} object_t;

object_t object_t$create( float _worldX, float _worldY );
void object_t$destroy( object_t* _object );

void object_t$step( object_t* _object, float _velocityX, float _velocityY );
void object_t$render( const object_t* _object,
                      const object_t* _camera,
                      bool _doDrawBoxes );

void object_t$add$state( object_t* _object,
                         SDL_Renderer* _renderer,
                         const char* _path,
                         const char* _pattern,
                         bool _isActionable,
                         bool _canLoop );
