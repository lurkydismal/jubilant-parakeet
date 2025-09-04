#pragma once

#include <SDL3/SDL_render.h>
#include <stdbool.h>

#include "state_t.h"

#define DEFAULT_OBJECT        \
    {                         \
        .stateNames = NULL,   \
        .states = NULL,       \
        .currentState = NULL, \
        .worldX = 0,          \
        .worldY = 0,          \
        .worldXMin = 0,       \
        .worldYMin = 0,       \
        .worldXMax = 0,       \
        .worldYMax = 0,       \
    }

typedef struct {
    char** stateNames;
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

// Boxes
// fileName_ColorAsHex.extension
// X Y Width Height StartIndex-EndIndex
// Animation
// fileName_WidthxHeight_StartIndex-EndIndex.extension
bool object_t$state$add$fromPaths( object_t* restrict _object,
                                   SDL_Renderer* _renderer,
                                   char* restrict _boxesPath,
                                   char* const* restrict _animationPaths,
                                   char* restrict _name,
                                   bool _isActionable,
                                   bool _canLoop );
bool object_t$state$add$fromGlob( object_t* restrict _object,
                                  SDL_Renderer* _renderer,
                                  const char* restrict _boxesGlob,
                                  const char* restrict _animationGlob,
                                  char* restrict _name,
                                  bool _isActionable,
                                  bool _canLoop );
bool object_t$state$remove$byName( object_t* restrict _object,
                                   char* restrict _name );
bool object_t$states$remove( object_t* restrict _object );
bool object_t$state$change$byName( object_t* restrict _object,
                                   char* restrict _name );

bool object_t$move( object_t* restrict _object,
                    const float _x,
                    const float _y );
bool object_t$step( object_t* restrict _object,
                    float _velocityX,
                    float _velocityY );
bool object_t$render$rotated( const object_t* restrict _object,
                              const double _angle,
                              SDL_FlipMode _flipMode,
                              const SDL_FRect* restrict _cameraRectangle,
                              bool _doDrawBoxes );
bool object_t$render( const object_t* restrict _object,
                      const SDL_FRect* restrict _cameraRectangle,
                      bool _doDrawBoxes );
