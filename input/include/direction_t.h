#pragma once

typedef enum {
    UP = ( 1 << 0 ),
    RIGHT = ( 1 << 1 ),
    DOWN = ( 1 << 2 ),
    LEFT = ( 1 << 3 ),
    UP_RIGHT = ( UP | RIGHT ),
    DOWN_RIGHT = ( DOWN | RIGHT ),
    DOWN_LEFT = ( DOWN | LEFT ),
    UP_LEFT = ( UP | LEFT )
} direction_t;
