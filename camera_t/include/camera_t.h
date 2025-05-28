#pragma once

#include <stdbool.h>

#include "object_t.h"

#define DEFAULT_CAMERA { .object = DEFAULT_OBJECT }

typedef struct {
    object_t object;
} camera_t;

camera_t camera_t$create( void );
bool camera_t$destroy( camera_t* _camera );
