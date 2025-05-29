#pragma once

#include <stdbool.h>

#include "player_t.h"

#define DEFAULT_CAMERA { .worldX = 0, .worldY = 0 }

typedef struct camera_t {
    float worldX;
    float worldY;
} camera_t;

camera_t camera_t$create( void );
bool camera_t$destroy( camera_t* _camera );

bool camera_t$update( player_t* _player );
