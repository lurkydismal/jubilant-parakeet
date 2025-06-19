#pragma once

#include <stddef.h>

#include "stdfunc.h"

#define INPUT_BUTTON_MASK ( A | B | C | D )

#define BUTTON_TYPE_CHAR_A 'A'
#define BUTTON_TYPE_CHAR_B 'B'
#define BUTTON_TYPE_CHAR_C 'C'
#define BUTTON_TYPE_CHAR_D 'D'

#define BUTTON_TYPE_TO_CHAR( _buttonType ) BUTTON_TYPE_CHAR_##_buttonType

typedef enum {
    A = ( 1 << 4 ),
    B = ( 1 << 5 ),
    C = ( 1 << 6 ),
    D = ( 1 << 7 ),
    BUTTON_COUNT = 4
} button_t;

static FORCE_INLINE const char* button_t$convert$toStaticString(
    button_t _button ) {
    static char l_returnValue[ BUTTON_COUNT + 1 ];

    if ( !_button ) {
        l_returnValue[ 0 ] = '\0';

        goto EXIT;
    }

    {
#define APPEND_IF_BUTTON_SET( _buffer, _bufferLength, _button, _buttonType ) \
    do {                                                                     \
        if ( ( _button ) & ( _buttonType ) ) {                               \
            ( _buffer )[ ( _bufferLength ) ] =                               \
                BUTTON_TYPE_TO_CHAR( _buttonType );                          \
            ( _button ) &= ~( _buttonType );                                 \
            ( _bufferLength )++;                                             \
        }                                                                    \
    } while ( 0 )

        size_t l_length = 0;

        APPEND_IF_BUTTON_SET( l_returnValue, l_length, _button, A );
        APPEND_IF_BUTTON_SET( l_returnValue, l_length, _button, B );
        APPEND_IF_BUTTON_SET( l_returnValue, l_length, _button, C );
        APPEND_IF_BUTTON_SET( l_returnValue, l_length, _button, D );

#undef APPEND_IF_BUTTON_SET

        l_returnValue[ l_length ] = '\0';
    }

EXIT:
    return ( l_returnValue );
}
