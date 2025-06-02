#pragma once

#include "stdfunc.h"

#define INPUT_DIRECTION_MASK ( UP | RIGHT | DOWN | LEFT )

#define DIRECTION_TYPE_CHAR_UP '8'
#define DIRECTION_TYPE_CHAR_DOWN '2'
#define DIRECTION_TYPE_CHAR_LEFT '4'
#define DIRECTION_TYPE_CHAR_RIGHT '6'
#define DIRECTION_TYPE_CHAR_UP_LEFT '7'
#define DIRECTION_TYPE_CHAR_UP_RIGHT '9'
#define DIRECTION_TYPE_CHAR_DOWN_LEFT '1'
#define DIRECTION_TYPE_CHAR_DOWN_RIGHT '3'

#define DIRECTION_TYPE_TO_CHAR( _directionType ) \
    DIRECTION_TYPE_CHAR_##_directionType

typedef enum {
    UP = ( 1 << 0 ),
    DOWN = ( 1 << 1 ),
    LEFT = ( 1 << 2 ),
    RIGHT = ( 1 << 3 ),
    UP_LEFT = ( UP | LEFT ),
    UP_RIGHT = ( UP | RIGHT ),
    DOWN_LEFT = ( DOWN | LEFT ),
    DOWN_RIGHT = ( DOWN | RIGHT ),
    DIRECITONS_COUNT = 8
} direction_t;

static FORCE_INLINE const char* direction_t$convert$toStaticString(
    direction_t _direction ) {
    static char l_returnValue[ DIRECITONS_COUNT + 1 ];

    if ( !_direction ) {
        l_returnValue[ 0 ] = '5';
        l_returnValue[ 1 ] = '\0';

        goto EXIT;
    }

    {
#define APPEND_IF_DIRECTION_MATCH( _buffer, _bufferLength, _direction,        \
                                   _directionType, _matchExactly )            \
    do {                                                                      \
        if ( ( _matchExactly ) ? ( ( ( _direction ) & ( _directionType ) ) == \
                                   ( _directionType ) )                       \
                               : ( ( _direction ) & ( _directionType ) ) ) {  \
            ( _buffer )[ ( _bufferLength ) ] =                                \
                DIRECTION_TYPE_TO_CHAR( _directionType );                     \
            ( _direction ) &= ~( _directionType );                            \
            ( _bufferLength )++;                                              \
        }                                                                     \
    } while ( 0 )

        size_t l_length = 0;

        APPEND_IF_DIRECTION_MATCH( l_returnValue, l_length, _direction, UP_LEFT,
                                   true );
        APPEND_IF_DIRECTION_MATCH( l_returnValue, l_length, _direction,
                                   UP_RIGHT, true );

        APPEND_IF_DIRECTION_MATCH( l_returnValue, l_length, _direction,
                                   DOWN_LEFT, true );
        APPEND_IF_DIRECTION_MATCH( l_returnValue, l_length, _direction,
                                   DOWN_RIGHT, true );

        APPEND_IF_DIRECTION_MATCH( l_returnValue, l_length, _direction, UP,
                                   false );
        APPEND_IF_DIRECTION_MATCH( l_returnValue, l_length, _direction, DOWN,
                                   false );
        APPEND_IF_DIRECTION_MATCH( l_returnValue, l_length, _direction, LEFT,
                                   false );
        APPEND_IF_DIRECTION_MATCH( l_returnValue, l_length, _direction, RIGHT,
                                   false );

#undef APPEND_IF_DIRECTION_MATCH

        l_returnValue[ l_length ] = '\0';
    }

EXIT:
    return ( l_returnValue );
}
