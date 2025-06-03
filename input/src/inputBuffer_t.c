#include "inputBuffer_t.h"

#include "stdfunc.h"

inputBuffer_t inputBuffer_t$create( void ) {
    inputBuffer_t l_returnValue = DEFAULT_INPUT_BUFFER;

    {
        __builtin_memset( l_returnValue.inputs, 0,
                          sizeof( l_returnValue.inputs ) );
        __builtin_memset( l_returnValue.frames, 0,
                          sizeof( l_returnValue.frames ) );
    }

    return ( l_returnValue );
}

bool inputBuffer_t$destroy( inputBuffer_t* _inputBuffer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        __builtin_memset( _inputBuffer->inputs, 0,
                          sizeof( _inputBuffer->inputs ) );
        __builtin_memset( _inputBuffer->frames, 0,
                          sizeof( _inputBuffer->frames ) );

        _inputBuffer->currentBufferIndex = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool inputBuffer_t$insert( inputBuffer_t* _inputBuffer,
                           input_t _input,
                           size_t _frame ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_input ) ) {
        goto EXIT;
    }

    {
        const size_t l_currentBufferIndex = _inputBuffer->currentBufferIndex;

#if 0
        // TODO: Implement
        const size_t l_previousBufferIndex =
            ( ( l_currentBufferIndex == 0 )
                  ? ( arrayLastElementIndexNative( _inputBuffer->frames ) )
                  : ( l_currentBufferIndex - 1 ) );

        if ( _inputBuffer->frames[ l_previousBufferIndex ] >= _frame ) {
            l_returnValue = false;

            goto EXIT;
        }
#endif

        _inputBuffer->inputs[ l_currentBufferIndex ] = _input;
        _inputBuffer->frames[ l_currentBufferIndex ] = _frame;

        _inputBuffer->currentBufferIndex =
            ( ( l_currentBufferIndex + 1 ) %
              arrayLengthNative( _inputBuffer->inputs ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE input_t* inputBuffer_t$inputsSequence$get$last(
    inputBuffer_t* _inputBuffer ) {
    input_t* l_returnValue = NULL;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        const size_t l_currentBufferIndex =
            ( _inputBuffer->currentBufferIndex - 1 );
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement current frame
input_t** inputBuffer_t$inputsSequence$get( inputBuffer_t* _inputBuffer ) {
    input_t** l_returnValue = NULL;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        input_t** l_buffer = createArray( input_t* );

        const size_t l_currentBufferIndex =
            ( ( _inputBuffer->currentBufferIndex )
                  ? ( _inputBuffer->currentBufferIndex - 1 )
                  : ( 0 ) );

        if ( LIKELY( l_currentBufferIndex > 0 ) ) {
            size_t l_frame = _inputBuffer->frames[ l_currentBufferIndex ];

            FOR_RANGE_REVERSE( ssize_t, ( l_currentBufferIndex - 1 ), -1 ) {
                size_t l_frameNext = _inputBuffer->frames[ _index ];

                if ( ( l_frameNext + MAX_DELAY_BETWEEN_INPUTS ) < l_frame ) {
                    const size_t l_index = _index;

                    FOR_RANGE( size_t, ( l_index + 1 ),
                               ( l_currentBufferIndex + 1 ) ) {
                        insertIntoArray( &l_buffer,
                                         &( _inputBuffer->inputs[ _index ] ) );
                    }

                    break;
                }

                l_frame = l_frameNext;
            }
        }

        if ( LIKELY( l_currentBufferIndex <
                     arrayLengthNative( _inputBuffer->inputs ) ) &&
             ( LIKELY( arrayLastElementNative( _inputBuffer->inputs ) ) ) ) {
            size_t l_frame = _inputBuffer->frames[ l_currentBufferIndex ];

            FOR_RANGE_REVERSE( size_t,
                               arrayLengthNative( _inputBuffer->inputs ),
                               l_currentBufferIndex ) {
                size_t l_frameNext = _inputBuffer->frames[ _index ];

                if ( ( l_frameNext + MAX_DELAY_BETWEEN_INPUTS ) < l_frame ) {
                    const size_t l_index = _index;

                    FOR_RANGE( size_t, ( l_index + 1 ),
                               ( l_currentBufferIndex + 1 ) ) {
                        insertIntoArray( &l_buffer,
                                         &( _inputBuffer->inputs[ _index ] ) );
                    }

                    break;
                }

                l_frame = l_frameNext;
            }
        }

        l_returnValue = l_buffer;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement current frame
// TODO: Implement
input_t** inputBuffer_t$inputsSequence$get$withLimit(
    inputBuffer_t* _inputBuffer,
    const size_t _limitAmount ) {
    input_t** l_returnValue = NULL;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_limitAmount ) ||
         UNLIKELY( _limitAmount >
                   arrayLengthNative( _inputBuffer->inputs ) ) ) {
        goto EXIT;
    }

    {
        input_t** l_buffer = createArray( input_t* );

        const size_t l_currentBufferIndex = _inputBuffer->currentBufferIndex;

        l_returnValue = l_buffer;
    }

EXIT:
    return ( l_returnValue );
}
