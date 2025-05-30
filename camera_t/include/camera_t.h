#pragma once

#include <SDL3/SDL_rect.h>
#include <stdbool.h>

#include "player_t.h"

#define DEFAULT_CAMERA \
    { .zoom = 1, .zoomMin = 0.1, .zoomMax = 5, .rectangle = { 0, 0, 0, 0 } }

typedef struct {
    float zoom;
    float zoomMin;
    float zoomMax;
    SDL_FRect rectangle;
} camera_t;

camera_t camera_t$create( void );
bool camera_t$destroy( camera_t* _camera );

bool camera_t$load( camera_t* _camera, const SDL_FRect* _targetRectangle );
bool camera_t$unload( camera_t* _camera );

bool camera_t$update( camera_t* _camera, player_t* _player );
