#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "button_t.h"
#include "direction_t.h"
#include "stdfunc.h"

#define MAX_DURATION_LENGTH ( lengthOfNumber( SIZE_MAX ) )

#define MAKE_INPUT( _direction, _button )                                \
    { .data = ( ( uint8_t )( ( ( _direction ) & INPUT_DIRECTION_MASK ) | \
                             ( ( _button ) & INPUT_BUTTON_MASK ) ) ),    \
      .duration = 0 }

#define GET_DIRECTION( _input ) \
    ( ( direction_t )( ( _input )->data & INPUT_DIRECTION_MASK ) )
#define GET_BUTTON( _input ) \
    ( ( button_t )( ( _input )->data & INPUT_BUTTON_MASK ) )
#define GET_DURATION( _input ) ( ( _input )->duration )

#define DEFAULT_INPUT { .data = 0, .duration = 0 }

typedef struct {
    uint8_t data;
    size_t duration;
} input_t;

input_t input_t$create( void );
bool input_t$destroy( input_t* restrict _input );

static FORCE_INLINE const char* input_t$convert$toStaticString(
    const input_t* restrict _input ) {
    static char l_returnValue[ BUTTON_COUNT + DIRECTIONS_COUNT + 1 +
                               MAX_DURATION_LENGTH + 1 + 1 ];

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

    // Duration
    {
        const size_t l_duration = GET_DURATION( _input );

        if ( LIKELY( l_duration ) ) {
            l_returnValue[ l_length ] = '[';
            l_length++;

            char* l_durationAsString = numberToString( l_duration );
            const size_t l_durationAsStringLength =
                lengthOfNumber( l_duration );

            __builtin_memcpy( ( l_returnValue + l_length ), l_durationAsString,
                              l_durationAsStringLength );

            free( l_durationAsString );

            l_length += l_durationAsStringLength;

            l_returnValue[ l_length ] = ']';
            l_length++;
        }
    }

    l_returnValue[ l_length ] = '\0';

    return ( l_returnValue );
}
