#include "input_t.h"

#include "stdfunc.h"

char* input_t$convert$toString( input_t _input ) {
    char* l_returnValue = NULL;

    if ( UNLIKELY( !_input ) ) {
        goto EXIT;
    }

    {
        char l_buffer[ ( 1 + 3 + 1 ) ];

        __builtin_memset( l_buffer, 0, arrayLengthNative( l_buffer ) );

        size_t l_length = 0;

        // Direction
        {
            const direction_t l_direction = GET_DIRECTION( _input );

            l_buffer[ l_length ] =
                direction_t$convert$toCharacter( l_direction );

            l_length++;
        }

        // Button
        {
            const button_t l_button = GET_BUTTON( _input );
            const char* l_buttonAsString =
                button_t$convert$toString( l_button );
            const size_t l_buttonAsStringLength =
                __builtin_strlen( l_buttonAsString );

            __builtin_memcpy( l_buffer, &l_buttonAsString,
                              l_buttonAsStringLength );

            l_length += l_buttonAsStringLength;
        }

        l_buffer[ l_length ] = '\0';

        l_returnValue = ( char* )malloc( l_length * sizeof( char ) );

        __builtin_memcpy( l_returnValue, &l_buffer, l_length );
    }

EXIT:
    return ( l_returnValue );
}
