#pragma once

#include <SDL3/SDL_pixels.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "stdfunc.h"

#define COLOR_BLACK \
    { .red = 0, .green = 0, .blue = 0, .alpha = SDL_ALPHA_OPAQUE }
#define COLOR_WHITE \
    { .red = 0xFF, .green = 0xFF, .blue = 0xFF, .alpha = SDL_ALPHA_OPAQUE }
#define COLOR_RED \
    { .red = 0xFF, .green = 0, .blue = 0, .alpha = SDL_ALPHA_OPAQUE }
#define COLOR_GREEN \
    { .red = 0, .green = 0xFF, .blue = 0, .alpha = SDL_ALPHA_OPAQUE }
#define COLOR_BLUE \
    { .red = 0, .green = 0, .blue = 0xFF, .alpha = SDL_ALPHA_OPAQUE }
#define DEFAULT_COLOR COLOR_WHITE

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} color_t;

// TODO: Add alpha
// Hex string
static FORCE_INLINE color_t
color_t$convert$fromString( const char* restrict _string ) {
    color_t l_returnValue = DEFAULT_COLOR;

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    {
        const size_t l_colorAsNumber = strtoul( _string, NULL, 16 );

        if ( UNLIKELY( !l_colorAsNumber ) || UNLIKELY( errno == ERANGE ) ) {
            goto EXIT;
        }

        typedef enum { red = 2, green = 1, blue = 0 } colorChannel_t;

#define GET_COLOR_FROM_NUMBER( _number, _colorChannel ) \
    ( ( ( _number ) >> ( 8 * _colorChannel ) ) & 0xFF )

        l_returnValue.red =
            GET_COLOR_FROM_NUMBER( l_colorAsNumber, ( colorChannel_t )red );
        l_returnValue.green =
            GET_COLOR_FROM_NUMBER( l_colorAsNumber, ( colorChannel_t )green );
        l_returnValue.blue =
            GET_COLOR_FROM_NUMBER( l_colorAsNumber, ( colorChannel_t )blue );

#undef GET_COLOR_FROM_NUMBER
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Improve
static FORCE_INLINE const char* color_t$convert$toStaticString(
    const color_t* restrict _color ) {
    static char l_returnValue[ 8 + 1 ];

    if ( UNLIKELY( !_color ) ) {
        l_returnValue[ 0 ] = '\0';

        goto EXIT;
    }

    {
        const char l_hexDigits[] = "0123456789ABCDEF";

        l_returnValue[ 0 ] = l_hexDigits[ ( _color->red >> 4 ) & 0xF ];
        l_returnValue[ 1 ] = l_hexDigits[ _color->red & 0xF ];
        l_returnValue[ 2 ] = l_hexDigits[ ( _color->green >> 4 ) & 0xF ];
        l_returnValue[ 3 ] = l_hexDigits[ _color->green & 0xF ];
        l_returnValue[ 4 ] = l_hexDigits[ ( _color->blue >> 4 ) & 0xF ];
        l_returnValue[ 5 ] = l_hexDigits[ _color->blue & 0xF ];
        l_returnValue[ 6 ] = l_hexDigits[ ( _color->alpha >> 4 ) & 0xF ];
        l_returnValue[ 7 ] = l_hexDigits[ _color->alpha & 0xF ];

        l_returnValue[ 8 ] = '\0';
    }

EXIT:
    return ( l_returnValue );
}
