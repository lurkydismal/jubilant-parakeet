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

static FORCE_INLINE size_t
inputBuffer_t$currentBufferIndex$get( inputBuffer_t* _inputBuffer ) {
    size_t l_returnValue = SIZE_MAX;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        if ( LIKELY( _inputBuffer->currentBufferIndex ) ) {
            l_returnValue = ( _inputBuffer->currentBufferIndex - 1 );

        } else {
            const size_t l_lastInputFrame =
                arrayLastElementNative( _inputBuffer->frames );

            if ( LIKELY( l_lastInputFrame ) ) {
                l_returnValue =
                    arrayLastElementIndexNative( _inputBuffer->frames );

            } else {
                l_returnValue = 0;
            }
        }
    }

EXIT:
    return ( l_returnValue );
}

size_t inputBuffer_t$inputsSequence$getFrame$last(
    inputBuffer_t* _inputBuffer ) {
    size_t l_returnValue = SIZE_MAX;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    {
        const size_t l_currentBufferIndex =
            inputBuffer_t$currentBufferIndex$get( _inputBuffer );

        if ( UNLIKELY( l_currentBufferIndex > INPUT_BUFFER_LENGTH ) ) {
            goto EXIT;
        }

        if ( LIKELY( l_currentBufferIndex ) ) {
            l_returnValue = _inputBuffer->frames[ l_currentBufferIndex ];

        } else {
            const size_t l_lastInputFrame =
                arrayLastElementNative( _inputBuffer->frames );

            if ( LIKELY( l_lastInputFrame ) ) {
                l_returnValue = l_lastInputFrame;

            } else {
                l_returnValue = arrayFirstElementNative( _inputBuffer->frames );
            }
        }
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
        // Do not insert input of the same or greater frame
        {
            const size_t l_lastInputFrame =
                inputBuffer_t$inputsSequence$getFrame$last( _inputBuffer );

            if ( UNLIKELY( l_lastInputFrame >= _frame ) ) {
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

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool
inputBuffer_t$inputsSequence$getInput$intoBuffer$byIndex(
    inputBuffer_t* _inputBuffer,
    input_t*** _buffer,
    size_t _index ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_buffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( _index >= arrayLengthNative( _inputBuffer->inputs ) ) ) {
        goto EXIT;
    }

    {
        input_t* l_input = &( _inputBuffer->inputs[ _index ] );

        if ( UNLIKELY( !( *l_input ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        insertIntoArray( _buffer, l_input );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool
inputBuffer_t$inputsSequence$getInputs$byRange$intoBuffer$forward(
    inputBuffer_t* _inputBuffer,
    input_t*** _buffer,
    size_t _startIndex,
    size_t _endIndex ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_buffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_startIndex ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( _endIndex > arrayLengthNative( _inputBuffer->inputs ) ) ) {
        goto EXIT;
    }

    {
        FOR_RANGE_REVERSE( ssize_t, _startIndex,
                           ( ( ssize_t )_endIndex - 1 ) ) {
            const size_t l_currentFrame = _inputBuffer->frames[ _index ];
            const size_t l_previousFrame = _inputBuffer->frames[ _index + 1 ];

            if ( ( l_currentFrame + MAX_DELAY_BETWEEN_INPUTS ) <
                 l_previousFrame ) {
                break;
            }

            l_returnValue =
                inputBuffer_t$inputsSequence$getInput$intoBuffer$byIndex(
                    _inputBuffer, _buffer, _index );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool
inputBuffer_t$inputsSequence$getInputs$byRange$intoBuffer$reverse(
    inputBuffer_t* _inputBuffer,
    input_t*** _buffer,
    size_t _startIndex,
    size_t _endIndex ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_buffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_startIndex ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( _endIndex > arrayLengthNative( _inputBuffer->inputs ) ) ) {
        goto EXIT;
    }

    {
        FOR_RANGE_REVERSE( ssize_t, _startIndex,
                           ( ( ssize_t )_endIndex - 1 ) ) {
            const size_t l_currentFrame = _inputBuffer->frames[ _index ];
            const size_t l_previousFrame = _inputBuffer->frames[ _index - 1 ];

            if ( ( l_currentFrame + MAX_DELAY_BETWEEN_INPUTS ) <
                 l_previousFrame ) {
                break;
            }

            l_returnValue =
                inputBuffer_t$inputsSequence$getInput$intoBuffer$byIndex(
                    _inputBuffer, _buffer, _index );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool
inputBuffer_t$inputsSequence$getInputs$byRange$intoBuffer(
    inputBuffer_t* _inputBuffer,
    input_t*** _buffer,
    size_t _startIndex,
    size_t _endIndex ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_buffer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_startIndex ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( _endIndex > arrayLengthNative( _inputBuffer->inputs ) ) ) {
        goto EXIT;
    }

    {
        if ( _startIndex > _endIndex ) {
            l_returnValue =
                inputBuffer_t$inputsSequence$getInputs$byRange$intoBuffer$forward(
                    _inputBuffer, _buffer, _startIndex, _endIndex );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

        } else if ( _startIndex < _endIndex ) {
#if 0
            l_returnValue =
                inputBuffer_t$inputsSequence$getInputs$byRange$intoBuffer$reverse(
                    _inputBuffer, _buffer, _endIndex, _startIndex );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
#endif

        } else if ( UNLIKELY( _startIndex == _endIndex ) ) {
            // One item
            const size_t l_index = _startIndex;

            l_returnValue =
                inputBuffer_t$inputsSequence$getInput$intoBuffer$byIndex(
                    _inputBuffer, _buffer, l_index );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement current frame
input_t** inputBuffer_t$inputsSequence$get( inputBuffer_t* _inputBuffer,
                                            const size_t _currentFrame ) {
    input_t** l_returnValue = NULL;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    // Do not accept frame 0
    if ( UNLIKELY( !_currentFrame ) ) {
        goto EXIT;
    }

    {
        input_t** l_buffer = createArray( input_t* );

        // Check last frame greater than current frame
        {
            const size_t l_lastInputFrame =
                inputBuffer_t$inputsSequence$getFrame$last( _inputBuffer );

            if ( UNLIKELY( l_lastInputFrame >= _currentFrame ) ) {
                l_returnValue = false;

                goto EXIT;
            }
        }

        {
            const size_t l_currentBufferIndex =
                ( _inputBuffer->currentBufferIndex - 1 );

            bool l_wasInserted =
                inputBuffer_t$inputsSequence$getInputs$byRange$intoBuffer(
                    _inputBuffer, &l_buffer, l_currentBufferIndex, 0 );

            if ( UNLIKELY( !l_wasInserted ) ) {
                goto EXIT_SEQUENCE_GET;
            }

            l_wasInserted =
                inputBuffer_t$inputsSequence$getInputs$byRange$intoBuffer(
                    _inputBuffer, &l_buffer,
                    arrayLengthNative( _inputBuffer->inputs ),
                    l_currentBufferIndex );

            if ( UNLIKELY( !l_wasInserted ) ) {
                goto EXIT_SEQUENCE_GET;
            }

        EXIT_SEQUENCE_GET:
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
    const size_t _currentFrame,
    const size_t _limitAmount ) {
    input_t** l_returnValue = NULL;

    if ( UNLIKELY( !_inputBuffer ) ) {
        goto EXIT;
    }

    // Do not accept frame 0
    if ( UNLIKELY( !_currentFrame ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_limitAmount ) ||
         UNLIKELY( _limitAmount >
                   arrayLengthNative( _inputBuffer->inputs ) ) ) {
        goto EXIT;
    }

    {
        input_t** l_buffer = createArray( input_t* );

        // Check last frame greater than current frame
        {
            const size_t l_lastInputFrame =
                inputBuffer_t$inputsSequence$getFrame$last( _inputBuffer );

            if ( UNLIKELY( l_lastInputFrame >= _currentFrame ) ) {
                l_returnValue = false;

                goto EXIT;
            }
        }

        l_returnValue = l_buffer;
    }

EXIT:
    return ( l_returnValue );
}
