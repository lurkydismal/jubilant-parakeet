#pragma once

#include <stdint.h>

#include "button_t.h"
#include "direction_t.h"

#define MAKE_INPUT( _direction, _button )                      \
    ( ( input_t )( ( ( _direction ) & INPUT_DIRECTION_MASK ) | \
                   ( ( _button ) & INPUT_BUTTON_MASK ) ) )

#define GET_DIRECTION( _input ) \
    ( ( direction_t )( ( _input ) & INPUT_DIRECTION_MASK ) )
#define GET_BUTTON( _input ) ( ( button_t )( ( _input ) & INPUT_BUTTON_MASK ) )

typedef uint8_t input_t;

static FORCE_INLINE const char* input_t$convert$toStaticString(
    const input_t _input ) {
    static char l_returnValue[ ( BUTTON_COUNT + DIRECTIONS_COUNT + 1 ) ];

    size_t l_length = 0;

    // Direction
    {
        const direction_t l_direction = GET_DIRECTION( _input );

        const char* l_directionAsString =
            direction_t$convert$toStaticString( l_direction );
        const size_t l_directionAsStringLength =
            __builtin_strlen( l_directionAsString );

        __builtin_memcpy( ( l_returnValue + l_length ), l_directionAsString,
                          l_directionAsStringLength );

        l_length += l_directionAsStringLength;
    }

    // Button
    {
        const button_t l_button = GET_BUTTON( _input );

        const char* l_buttonAsString =
            button_t$convert$toStaticString( l_button );
        const size_t l_buttonAsStringLength =
            __builtin_strlen( l_buttonAsString );

        __builtin_memcpy( ( l_returnValue + l_length ), l_buttonAsString,
                          l_buttonAsStringLength );

        l_length += l_buttonAsStringLength;
    }

    l_returnValue[ l_length ] = '\0';

    return ( l_returnValue );
}
