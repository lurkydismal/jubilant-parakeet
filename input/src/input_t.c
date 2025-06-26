#include "input_t.h"

input_t input_t$create( void ) {
    input_t l_returnValue = DEFAULT_INPUT;

    return ( l_returnValue );
}

bool input_t$destroy( input_t* restrict _input ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_input ) ) {
        goto EXIT;
    }

    {
        _input->data = 0;
        _input->duration = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
