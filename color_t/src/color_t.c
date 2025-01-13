#include "color_t.h"

color_t color_t$fromString( const char* _string, Uint16 _base ) {
    color_t l_returnValue = DEFAULT_COLOR;

    if ( _string ) {
        const size_t l_colorAsNumber = SDL_strtoul( _string, NULL, _base );

        l_returnValue.red = ( ( l_colorAsNumber & 0xFF0000 ) >> ( 8 * 2 ) );
        l_returnValue.green = ( ( l_colorAsNumber & 0x00FF00 ) >> ( 8 * 1 ) );
        l_returnValue.blue = ( ( l_colorAsNumber & 0x0000FF ) >> ( 8 * 0 ) );

        return ( l_returnValue );
    }
}
