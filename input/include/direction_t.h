#pragma once

#include "stdfunc.h"

#define INPUT_DIRECTION_MASK ( UP | RIGHT | DOWN | LEFT )

typedef enum {
    UP = ( 1 << 0 ),
    DOWN = ( 1 << 1 ),
    LEFT = ( 1 << 2 ),
    RIGHT = ( 1 << 3 ),
    UP_LEFT = ( UP | LEFT ),
    UP_RIGHT = ( UP | RIGHT ),
    DOWN_LEFT = ( DOWN | LEFT ),
    DOWN_RIGHT = ( DOWN | RIGHT )
} direction_t;

static FORCE_INLINE char direction_t$convert$toCharacter(
    direction_t _direction ) {
    switch ( _direction ) {
        case UP: {
            return ( '8' );
        }

        case DOWN: {
            return ( '2' );
        }

        case LEFT: {
            return ( '4' );
        }

        case RIGHT: {
            return ( '6' );
        }

        case UP_LEFT: {
            return ( '7' );
        }

        case UP_RIGHT: {
            return ( '9' );
        }

        case DOWN_LEFT: {
            return ( '1' );
        }

        case DOWN_RIGHT: {
            return ( '3' );
        }

        default: {
            return ( '5' );
        }
    }
}
