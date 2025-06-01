#include "input_t.h"

#include "stdfunc.h"

// TODO: Improve
char* input_t$convert$toString( input_t _input ) {
    char* l_returnValue = NULL;

    if ( UNLIKELY( !_input ) ) {
        goto EXIT;
    }

    {
        char* l_buffer = ( char* )malloc( ( 1 + 3 + 1 ) * sizeof( char ) );

        size_t l_length = 0;

        const direction_t l_direction = GET_DIRECTION( _input );
        const button_t l_button = GET_BUTTON( _input );

        // Direction
        {
            switch ( l_direction ) {
                case UP: {
                    l_buffer[ l_length ] = '8';

                    break;
                }

                case DOWN: {
                    l_buffer[ l_length ] = '2';

                    break;
                }

                case LEFT: {
                    l_buffer[ l_length ] = '4';

                    break;
                }

                case RIGHT: {
                    l_buffer[ l_length ] = '6';

                    break;
                }

                case UP_LEFT: {
                    l_buffer[ l_length ] = '7';

                    break;
                }

                case UP_RIGHT: {
                    l_buffer[ l_length ] = '9';

                    break;
                }

                case DOWN_LEFT: {
                    l_buffer[ l_length ] = '1';

                    break;
                }

                case DOWN_RIGHT: {
                    l_buffer[ l_length ] = '3';

                    break;
                }

                default: {
                    l_buffer[ l_length ] = '5';
                }
            }
        }

        l_length++;

        // Button
        {
            switch ( l_button ) {
                case A: {
                    l_buffer[ l_length ] = 'A';

                    break;
                }

                case B: {
                    l_buffer[ l_length ] = 'B';

                    break;
                }

                case C: {
                    l_buffer[ l_length ] = 'C';

                    break;
                }

                case D: {
                    l_buffer[ l_length ] = 'D';

                    break;
                }

                case AB: {
                    __builtin_memcpy( l_buffer, "AB", 2 );
                    l_length++;

                    break;
                }

                case ABC: {
                    __builtin_memcpy( l_buffer, "ABC", 3 );
                    l_length++;
                    l_length++;

                    break;
                }
            }
        }

        l_length++;

        l_buffer[ l_length ] = '\0';

        l_returnValue = l_buffer;
    }

EXIT:
    return ( l_returnValue );
}
