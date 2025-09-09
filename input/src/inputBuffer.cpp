#include "inputBuffer.hpp"

#if 0

input_t** inputBuffer_t$inputsSequence$get$withLimit(
    inputBuffer_t* _inputBuffer,
    const size_t _currentFrame,
    const size_t _limitAmount ) {
    input_t** l_returnValue = NULL;

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
                log$transaction$query( ( logLevel_t )error,
                                       "Current frame is too old" );

                goto EXIT;
            }
        }

        const size_t l_previousBufferIndex = _inputBuffer->previousBufferIndex;
        size_t l_amountLeft = _limitAmount;

        size_t l_previousFrame = _currentFrame;

        // Previous to begin
        {
            FOR_RANGE_REVERSE( ssize_t, l_previousBufferIndex, ( 0 - 1 ) ) {
                if ( UNLIKELY( !l_amountLeft ) ) {
                    goto EXIT;
                }

                input_t* l_input = &( _inputBuffer->inputs[ _index ] );
                const size_t l_frame = _inputBuffer->frames[ _index ];

                if ( ( l_frame + MAX_DELAY_BETWEEN_INPUTS + 1 ) <
                     l_previousFrame ) {
                    break;
                }

                l_previousFrame = l_frame;

                if ( !l_input->data ) {
                    continue;
                }

                insertIntoArray( &l_returnValue, l_input );

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

                if ( ( l_frame + MAX_DELAY_BETWEEN_INPUTS + 1 ) <
                     l_previousFrame ) {
                    break;
                }

                l_previousFrame = l_frame;

                if ( !l_input->data ) {
                    continue;
                }

                insertIntoArray( &l_returnValue, l_input );

                l_amountLeft--;
            }
        }

        if ( !arrayLength( l_returnValue ) ) {
            input_t* l_lastInput =
                inputBuffer_t$inputsSequence$getInput$last( _inputBuffer );

            if ( l_lastInput->data ) {
                insertIntoArray( &l_returnValue, l_lastInput );
            }
        }
    }

EXIT:
    return ( l_returnValue );
}
#endif
