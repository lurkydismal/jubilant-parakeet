#include "inputBuffer_t.h"

inputBuffer_t inputBuffer_t$create(void) {
    inputBuffer_t l_returnValue = DEFAULT_INPUT_BUFFER;

    return ( l_returnValue );
}

bool inputBuffer_t$destroy( inputBuffer_t* _inputBuffer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool inputBuffer_t$insert( inputBuffer_t* _inputBuffer, input_t _input, size_t _frame ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_input ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
