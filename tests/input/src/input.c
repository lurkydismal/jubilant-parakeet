#include "inputBuffer_t.h"
#include "test.h"

TEST( direction_t$convert$toStaticString ) {
    // clang-format off
    direction_t l_directions[] = {
        UP, DOWN, LEFT, RIGHT,
        UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT,
        (UP | DOWN),
        (LEFT | RIGHT),
        (UP | DOWN | LEFT),
        (UP | DOWN | RIGHT),
        (LEFT | RIGHT | UP),
        (LEFT | RIGHT | DOWN),
        (UP | DOWN | LEFT | RIGHT),
        0
    };
    // clang-format on

    // clang-format off
    const char* l_directionsConverted[] = {
        "8",  // UP
        "2",  // DOWN
        "4",  // LEFT
        "6",  // RIGHT
        "7",  // UP_LEFT
        "9",  // UP_RIGHT
        "1",  // DOWN_LEFT
        "3",  // DOWN_RIGHT
        "82", // UP | DOWN
        "46", // LEFT | RIGHT
        "72", // UP | DOWN | LEFT
        "92", // UP | DOWN | RIGHT
        "76", // LEFT | RIGHT | UP
        "16", // LEFT | RIGHT | DOWN
        "73", // UP | DOWN | LEFT | RIGHT
        "5"   // No direction
    };
    // clang-format on

    FOR_RANGE( size_t, 0, arrayLengthNative( l_directions ) ) {
        ASSERT_STRING_EQ(
            direction_t$convert$toStaticString( l_directions[ _index ] ),
            l_directionsConverted[ _index ] );
    }
}

TEST( button_t$convert$toStaticString ) {
    // clang-format off
    button_t l_buttons[] = {
        A, B, C, D,
        (A|B), (A|C), (A|D),
        (B|C), (B|D),
        (C|D),
        (A|B|C), (A|C|D), (B|C|D),
        (A|B|C|D)
    };
    // clang-format on

    // clang-format off
    const char* l_buttonsConverted[] = {
        "A", "B", "C", "D",
        "AB", "AC", "AD",
        "BC", "BD",
        "CD",
        "ABC", "ACD", "BCD",
        "ABCD"
    };
    // clang-format on

    FOR_RANGE( size_t, 0, arrayLengthNative( l_buttons ) ) {
        ASSERT_STRING_EQ(
            button_t$convert$toStaticString( l_buttons[ _index ] ),
            l_buttonsConverted[ _index ] );
    }
}

// TODO: Implement
TEST( input_t$GET_DIRECTION ) {
    input_t l_input = MAKE_INPUT( UP, A );

    ASSERT_EQ( "%#b", GET_DIRECTION( l_input ), UP );
}

// TODO: Implement
TEST( input_t$GET_BUTTON ) {
    input_t l_input = MAKE_INPUT( UP, A );

    ASSERT_EQ( "%#b", GET_BUTTON( l_input ), A );
}

// TODO: Implement
TEST( input_t$MAKE_INPUT ) {
    input_t l_input = MAKE_INPUT( UP, A );

    ASSERT_EQ( "%#b", l_input, 0b10001 );
}

// TODO: Implement
TEST( input_t$convert$toStaticString ) {
    // clang-format off
    direction_t l_directions[] = {
        UP, DOWN, LEFT, RIGHT,
    };

    button_t l_buttons[] = {
        A, B, C, D,
    };

    const char* l_directionsConverted[] = {
        "8",  // UP
        "2",  // DOWN
        "4",  // LEFT
        "6",  // RIGHT
    };

    const char* l_buttonsConverted[] = {
        "A", "B", "C", "D",
    };
    // clang-format on

    ASSERT_STRING_EQ( input_t$convert$toStaticString(
                          MAKE_INPUT( ( UP | DOWN ), ( A | B | C ) ) ),
                      "82ABC" );
}

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
            const size_t l_frame = 0;

            bool l_returnValue =
                inputBuffer_t$insert( &l_inputBuffer, l_input, l_frame );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ( "%u", l_inputBuffer.inputs[ 0 ], l_input );
            ASSERT_EQ( "%zu", l_inputBuffer.frames[ 0 ], l_frame );
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
            ASSERT_EQ( "%zu", l_inputBuffer.frames[ 1 ], l_frame );
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
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );
                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )0 );
            }

            const input_t l_inputDummy1 = MAKE_INPUT( UP, A );
            const input_t l_inputDummy2 =
                MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) );

            // Insert
            {
                {
                    const size_t l_frame = 0;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%u", l_inputBuffer.inputs[ 0 ], l_inputDummy1 );
                    ASSERT_EQ( "%zu", l_inputBuffer.frames[ 0 ], l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )1 );
                }

                // Size max frame
                {
                    const size_t l_frame = 20;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, l_inputDummy2, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%u", l_inputBuffer.inputs[ 1 ], l_inputDummy2 );
                    ASSERT_EQ( "%zu", l_inputBuffer.frames[ 1 ], l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )2 );
                }
            }

            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )1 );

                {
                    const input_t* l_input = arrayFirstElement( l_inputs );

                    ASSERT_EQ( "%#b", GET_DIRECTION( *l_input ),
                               GET_DIRECTION( l_inputDummy2 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( *l_input ),
                               GET_BUTTON( l_inputDummy2 ) );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( *l_input ),
                        input_t$convert$toStaticString( l_inputDummy2 ) );
                }

                FREE_ARRAY( l_inputs );
            }

            // Insert one more and close by frame
            {
                const size_t l_frame = 25;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ( "%u", l_inputBuffer.inputs[ 2 ], l_inputDummy1 );
                ASSERT_EQ( "%zu", l_inputBuffer.frames[ 2 ], l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( size_t )3 );
            }

            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )2 );

                // First
                {
                    const input_t* l_input = arrayFirstElement( l_inputs );

                    ASSERT_EQ( "%#b", GET_DIRECTION( *l_input ),
                               GET_DIRECTION( l_inputDummy2 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( *l_input ),
                               GET_BUTTON( l_inputDummy2 ) );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( *l_input ),
                        input_t$convert$toStaticString( l_inputDummy2 ) );
                }

                // Second
                {
                    const input_t* l_input = arrayLastElement( l_inputs );

                    ASSERT_EQ( "%#b", GET_DIRECTION( *l_input ),
                               GET_DIRECTION( l_inputDummy1 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( *l_input ),
                               GET_BUTTON( l_inputDummy1 ) );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( *l_input ),
                        input_t$convert$toStaticString( l_inputDummy1 ) );
                }

                FREE_ARRAY( l_inputs );
            }
        }

        // Invalid
        {
            // NULL input buffer
            {
                input_t** l_inputs = inputBuffer_t$inputsSequence$get( NULL );

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
                const size_t l_frame = 0;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ( "%u", l_inputBuffer.inputs[ 0 ], l_inputDummy1 );
                ASSERT_EQ( "%zu", l_inputBuffer.frames[ 0 ], l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( size_t )1 );
            }

            // Up to limit
            {
                size_t l_count = 0;

                FOR_RANGE(
                    size_t, MAX_DELAY_BETWEEN_INPUTS,
                    ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS ) ) {
                    const size_t l_frame = _index;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%u", arrayLastElement( l_inputBuffer.inputs ),
                               l_inputDummy1 );
                    ASSERT_EQ( "%zu", arrayLastElement( l_inputBuffer.frames ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               l_count );
                }
            }
        }

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }
}

#if 0
TEST(InputBufferTest) {
    {
        SCOPED_TRACE("Contiguous inputs within MAX delay");

        inputBuffer_t buffer = inputBuffer_t$create();

        for (size_t i = 0; i < 5; ++i) {
            inputBuffer_t$insert(&buffer, (uint8_t)i, i * 2);  // spacing = 2
        }

        input_t** sequence = inputBuffer_t$inputsSequence$get(&buffer);

        ASSERT_NE(sequence, nullptr);
        ASSERT_EQ(arrayLength(sequence), 5u);

        FOR_RANGE(size_t, 0, 5) {
            ASSERT_EQ(*sequence[_index], _index);
        }

        freeInputSequence(sequence);
    }

    {
        SCOPED_TRACE("Skips old inputs after large frame gap");

        inputBuffer_t buffer = inputBuffer_t$create();

        inputBuffer_t$insert(&buffer, 1, 0);   // Frame 0
        inputBuffer_t$insert(&buffer, 2, 1);   // Frame 1
        inputBuffer_t$insert(&buffer, 3, 2);   // Frame 2
        inputBuffer_t$insert(&buffer, 4, 20);  // Frame 20
        inputBuffer_t$insert(&buffer, 5, 21);  // Frame 21

        input_t** sequence = inputBuffer_t$inputsSequence$get(&buffer);

        ASSERT_NE(sequence, nullptr);
        ASSERT_EQ(arrayLength(sequence), 2u);
        ASSERT_EQ(*sequence[0], 4);
        ASSERT_EQ(*sequence[1], 5);

        freeInputSequence(sequence);
    }

    {
        SCOPED_TRACE("No inputs returns empty array");

        inputBuffer_t buffer = inputBuffer_t$create();

        input_t** sequence = inputBuffer_t$inputsSequence$get(&buffer);

        ASSERT_NE(sequence, nullptr);
        ASSERT_EQ(arrayLength(sequence), 0u);

        freeInputSequence(sequence);
    }
}
#endif
