#include "inputBuffer_t.h"

#include "test.h"

TEST( inputBuffer_t$create ) {
    inputBuffer_t l_inputBuffer = inputBuffer_t$create();

    bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

    ASSERT_TRUE( l_returnValue );
}

TEST( inputBuffer_t$destroy ) {
    // Invalid - NULL input
    {
        bool l_returnValue = inputBuffer_t$destroy( NULL );

        ASSERT_FALSE( l_returnValue );
    }

    // Valid
    {
        inputBuffer_t l_inputBuffer = inputBuffer_t$create();

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }
}

// TODO: Wrap test
TEST( inputBuffer_t$insert ) {
    inputBuffer_t l_inputBuffer = inputBuffer_t$create();

    // Valid
    {
        {
            const input_t l_input = MAKE_INPUT( UP, A );
            const size_t l_frame = 1;

            bool l_returnValue =
                inputBuffer_t$insert( &l_inputBuffer, l_input, l_frame );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ( "%u", l_inputBuffer.inputs[ 0 ], l_input );
            ASSERT_EQ(
                "%zu",
                inputBuffer_t$inputsSequence$getFrame$last( &l_inputBuffer ),
                l_frame );
            ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex, ( size_t )1 );
        }

        // Size max frame
        {
            const input_t l_input =
                MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) );
            const size_t l_frame = SIZE_MAX;

            bool l_returnValue =
                inputBuffer_t$insert( &l_inputBuffer, l_input, l_frame );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ( "%u", l_inputBuffer.inputs[ 1 ], l_input );
            ASSERT_EQ(
                "%zu",
                inputBuffer_t$inputsSequence$getFrame$last( &l_inputBuffer ),
                l_frame );
            ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex, ( size_t )2 );
        }
    }

    // Invalid
    {
        // NULL input buffer
        {
            bool l_returnValue =
                inputBuffer_t$insert( NULL, MAKE_INPUT( UP, A ), 0 );

            ASSERT_FALSE( l_returnValue );
        }

        // NULL input
        {
            bool l_returnValue = inputBuffer_t$insert( &l_inputBuffer, 0, 0 );

            ASSERT_FALSE( l_returnValue );
        }
    }

    bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

    ASSERT_TRUE( l_returnValue );
}

TEST( inputBuffer_t$inputsSequence$get ) {
    // No wrap
    {
        inputBuffer_t l_inputBuffer = inputBuffer_t$create();

        // Valid
        {
            // Empty buffer
            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer, 1 );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )0 );

                FREE_ARRAY( l_inputs );
            }

            const input_t l_inputDummy1 = MAKE_INPUT( UP, A );
            const input_t l_inputDummy2 =
                MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) );

            // Insert
            {
                {
                    const size_t l_frame = 1;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%u", inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer), l_inputDummy1 );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )1 );
                }

                {
                    const size_t l_frame = 20;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, l_inputDummy2, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%u", inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer), l_inputDummy2 );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )2 );
                }
            }

            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer, 21 );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )1 );

                {
                    const input_t* l_input = arrayFirstElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( *l_input ),
                        input_t$convert$toStaticString( l_inputDummy2 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( *l_input ),
                               GET_DIRECTION( l_inputDummy2 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( *l_input ),
                               GET_BUTTON( l_inputDummy2 ) );
                }

                FREE_ARRAY( l_inputs );
            }

            // Insert one more and close by frame
            {
                const size_t l_frame = 25;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ( "%u", inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer), l_inputDummy1 );
                ASSERT_EQ( "%zu",
                           inputBuffer_t$inputsSequence$getFrame$last(
                               &l_inputBuffer ),
                           l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( size_t )3 );
            }

            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer, 26 );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )2 );

                // First
                {
                    const input_t* l_input = arrayFirstElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( *l_input ),
                        input_t$convert$toStaticString( l_inputDummy1 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( *l_input ),
                               GET_DIRECTION( l_inputDummy1 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( *l_input ),
                               GET_BUTTON( l_inputDummy1 ) );
                }

                // Second
                {
                    const input_t* l_input = arrayLastElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( *l_input ),
                        input_t$convert$toStaticString( l_inputDummy1 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( *l_input ),
                               GET_DIRECTION( l_inputDummy2 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( *l_input ),
                               GET_BUTTON( l_inputDummy2 ) );
                }

                FREE_ARRAY( l_inputs );
            }
        }

        // Invalid
        {
            // NULL input buffer
            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( NULL, 1 );

                ASSERT_EQ( "%p", l_inputs, NULL );
            }

            // Zero current frame
            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer, 0 );

                ASSERT_EQ( "%p", l_inputs, NULL );
            }
        }

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }

    // Wrap
    {
        inputBuffer_t l_inputBuffer = inputBuffer_t$create();

        const input_t l_inputDummy1 = MAKE_INPUT( UP, A );

        // Insert
        {
            {
                const size_t l_frame = 1;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ( "%#b", inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer), l_inputDummy1 );
                ASSERT_EQ( "%zu",
                           inputBuffer_t$inputsSequence$getFrame$last(
                               &l_inputBuffer ),
                           l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( size_t )1 );
            }

            // Up to limit
            {
                FOR_RANGE( size_t, MAX_DELAY_BETWEEN_INPUTS,
                           ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS -
                             1 - 1 ) ) {
                    const size_t l_frame = _index;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    const size_t l_indexInBuffer =
                        ( _index - MAX_DELAY_BETWEEN_INPUTS + 1 );

                    ASSERT_EQ( "%#b", inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer),
                               l_inputDummy1 );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( l_indexInBuffer + 1 ) );

                    printf( "%zu ", _index );
                }
                printf( "\n" );
            }

            // Over limit
            {
                {
                    const size_t l_frame =
                        ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS - 1 );

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    const size_t l_indexInBuffer = ( INPUT_BUFFER_LENGTH - 1 );

                    ASSERT_EQ( "%#b", l_inputBuffer.inputs[ l_indexInBuffer ],
                               l_inputDummy1 );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )0 );
                }

                {
                    input_t** l_inputs = inputBuffer_t$inputsSequence$get(
                        &l_inputBuffer,
                        ( MAX_DELAY_BETWEEN_INPUTS + INPUT_BUFFER_LENGTH +
                          MAX_DELAY_BETWEEN_INPUTS ) );

                    ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                    size_t c = 0;
                    printf( "\n%zu\n", c );
                    FOR( size_t*, l_inputBuffer.frames ) {
                        printf( "%zu ", *_element );
                        c++;
                    }

                    printf( "\n%zu\n", c );
                    printf( "TEST %zu\n",
                            inputBuffer_t$inputsSequence$getFrame$last(
                                &l_inputBuffer ) );
                    ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )100 );

                    FREE_ARRAY( l_inputs );
                }

                {
                    const size_t l_frame =
                        ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS - 1 );

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    const size_t l_indexInBuffer = ( 0 );

                    ASSERT_EQ( "%#b", l_inputBuffer.inputs[ l_indexInBuffer ],
                               l_inputDummy1 );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )1 );
                }
            }
        }

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }
}
