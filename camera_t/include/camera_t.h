#pragma once

#include <stdbool.h>

#include "player_t.h"

#define DEFAULT_CAMERA { .worldX = 0, .worldY = 0 }

typedef struct camera_t {
    float zoom;
    float zoomMin;
    float zoomMax;
    float worldX;
    float worldY;
    float width;
    float height;
    float logicalWidth;
    float logicalHeight;
} camera_t;

camera_t camera_t$create( void );
bool camera_t$destroy( camera_t* _camera );

bool camera_t$update( camera_t* _camera, player_t* _player );
