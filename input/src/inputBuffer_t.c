#include "inputBuffer_t.h"

#include "log.h"
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
        _inputBuffer->previousBufferIndex = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

input_t inputBuffer_t$inputsSequence$getInput$last(
    inputBuffer_t* _inputBuffer ) {
    input_t l_returnValue = 0;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        l_returnValue =
            _inputBuffer->inputs[ _inputBuffer->previousBufferIndex ];
    }

EXIT:
    return ( l_returnValue );
}

size_t inputBuffer_t$inputsSequence$getFrame$last(
    inputBuffer_t* _inputBuffer ) {
    size_t l_returnValue = 0;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        l_returnValue =
            _inputBuffer->frames[ _inputBuffer->previousBufferIndex ];
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
        // Check last frame greater than frame
        {
            const size_t l_lastInputFrame =
                inputBuffer_t$inputsSequence$getFrame$last( _inputBuffer );

            if ( UNLIKELY( l_lastInputFrame >= _frame ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Inserting into input buffer\n" );

                l_returnValue = false;

                goto EXIT;
            }
        }

        const size_t l_currentBufferIndex = _inputBuffer->currentBufferIndex;

        _inputBuffer->inputs[ l_currentBufferIndex ] = _input;
        _inputBuffer->frames[ l_currentBufferIndex ] = _frame;

        _inputBuffer->currentBufferIndex =
            ( ( l_currentBufferIndex + 1 ) %
              arrayLengthNative( _inputBuffer->inputs ) );

        _inputBuffer->previousBufferIndex = l_currentBufferIndex;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

input_t** inputBuffer_t$inputsSequence$get( inputBuffer_t* _inputBuffer,
                                            const size_t _currentFrame ) {
    input_t** l_returnValue = NULL;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        l_returnValue = createArray( input_t* );

        // No inputs in sequence
        if ( UNLIKELY( _inputBuffer->previousBufferIndex ==
                       _inputBuffer->currentBufferIndex ) ) {
            goto EXIT;
        }

        // Check last frame greater than current frame
        {
            const size_t l_lastInputFrame =
                inputBuffer_t$inputsSequence$getFrame$last( _inputBuffer );

            if ( UNLIKELY( l_lastInputFrame >= _currentFrame ) ) {
                goto EXIT;
            }
        }

        const size_t l_previousBufferIndex = _inputBuffer->previousBufferIndex;

        size_t l_previousFrame = _currentFrame;

        // Previous to begin
        {
            FOR_RANGE_REVERSE( ssize_t, l_previousBufferIndex, ( 0 - 1 ) ) {
                input_t* l_input = &( _inputBuffer->inputs[ _index ] );
                const size_t l_frame = _inputBuffer->frames[ _index ];

                if ( ( l_frame + MAX_DELAY_BETWEEN_INPUTS ) <
                     l_previousFrame ) {
                    goto EXIT;
                }

                insertIntoArray( &l_returnValue, l_input );

                l_previousFrame = l_frame;
            }
        }

        // End to previous
        {
            FOR_RANGE_REVERSE(
                size_t, ( arrayLengthNative( _inputBuffer->inputs ) - 1 ),
                l_previousBufferIndex ) {
                input_t* l_input = &( _inputBuffer->inputs[ _index ] );
                const size_t l_frame = _inputBuffer->frames[ _index ];

                if ( ( l_frame + MAX_DELAY_BETWEEN_INPUTS ) <
                     l_previousFrame ) {
                    goto EXIT;
                }

                insertIntoArray( &l_returnValue, l_input );

                l_previousFrame = l_frame;
            }
        }
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
input_t** inputBuffer_t$inputsSequence$get$withLimit(
    inputBuffer_t* _inputBuffer,
    const size_t _currentFrame,
    const size_t _limitAmount ) {
    input_t** l_returnValue = NULL;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        l_returnValue = createArray( input_t* );

        // No inputs in sequence
        if ( UNLIKELY( _inputBuffer->previousBufferIndex ==
                       _inputBuffer->currentBufferIndex ) ) {
            goto EXIT;
        }

        // Check last frame greater than current frame
        {
            const size_t l_lastInputFrame =
                inputBuffer_t$inputsSequence$getFrame$last( _inputBuffer );

            if ( UNLIKELY( l_lastInputFrame >= _currentFrame ) ) {
                goto EXIT;
            }
        }

        const size_t l_previousBufferIndex = _inputBuffer->previousBufferIndex;
        size_t l_amountLeft = _limitAmount;

        size_t l_previousFrame = _currentFrame;

        // Previous to begin
        {
            FOR_RANGE_REVERSE( ssize_t, l_previousBufferIndex, ( 0 - 1 ) ) {
                input_t* l_input = &( _inputBuffer->inputs[ _index ] );
                if ( UNLIKELY( !l_amountLeft ) ) {
                    goto EXIT;
                }

                const size_t l_frame = _inputBuffer->frames[ _index ];

                if ( ( l_frame + MAX_DELAY_BETWEEN_INPUTS ) <
                     l_previousFrame ) {
                    goto EXIT;
                }

                insertIntoArray( &l_returnValue, l_input );

                l_previousFrame = l_frame;

                l_amountLeft--;
            }
        }

        // End to previous
        {
            FOR_RANGE_REVERSE(
                size_t, ( arrayLengthNative( _inputBuffer->inputs ) - 1 ),
                l_previousBufferIndex ) {
                if ( UNLIKELY( !l_amountLeft ) ) {
                    goto EXIT;
                }

                input_t* l_input = &( _inputBuffer->inputs[ _index ] );
                const size_t l_frame = _inputBuffer->frames[ _index ];

                if ( ( l_frame + MAX_DELAY_BETWEEN_INPUTS ) <
                     l_previousFrame ) {
                    goto EXIT;
                }

                insertIntoArray( &l_returnValue, l_input );

                l_previousFrame = l_frame;

                l_amountLeft--;
            }
        }
    }

EXIT:
    return ( l_returnValue );
}
