#include "input_t.h"

#include "test.h"

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

    ASSERT_STRING_EQ( input_t$convert$toStaticString(
                          MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) ) ),
                      "46ABD" );
}
