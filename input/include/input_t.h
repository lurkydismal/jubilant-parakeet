#pragma once

#include <stdint.h>

#include "button_t.h"
#include "direction_t.h"

#define INPUT_BUTTON_SHIFT ( __builtin_ctz( INPUT_DIRECTION_MASK ) )

#define MAKE_INPUT( _direction, _button )                      \
    ( ( input_t )( ( ( _direction ) & INPUT_DIRECTION_MASK ) | \
                   ( ( ( _button ) & INPUT_BUTTON_SHIFT )      \
                     << INPUT_BUTTON_SHIFT ) ) )

#define GET_DIRECTION( _input ) ( ( _input ) & INPUT_DIRECTION_MASK )
#define GET_BUTTON( _input ) \
    ( ( ( _input ) & INPUT_BUTTON_MASK ) >> INPUT_BUTTON_SHIFT )

typedef uint8_t input_t;

char* input_t$convert$toString( input_t _input );
