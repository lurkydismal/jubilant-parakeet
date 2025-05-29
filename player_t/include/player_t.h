#pragma once

#include <stdbool.h>

#include "object_t.h"

// TODO: Improve
typedef struct camera_t camera_t;

#define DEFAULT_PLAYER { .object = DEFAULT_OBJECT }

typedef struct {
    object_t object;
} player_t;

player_t player_t$create( void );
bool player_t$destroy( player_t* _player );

// First file - boxes
// fileName_ColorAsHex.extension
// X Y Width Height StartIndex-EndIndex
// After - animation
// fileName_WidthxHeight_StartIndex-EndIndex.extension
bool player_t$state$add$fromFiles( player_t* restrict _player,
                                   SDL_Renderer* _renderer,
                                   char* const* restrict _files,
                                   bool _isActionable,
                                   bool _canLoop );

bool player_t$move( player_t* restrict _player, float _x, float _y );
bool player_t$step( player_t* restrict _player,
                    float _velocityX,
                    float _velocityY );
bool player_t$render( const player_t* restrict _player,
                      const camera_t* restrict _camera,
                      bool _doDrawBoxes );
