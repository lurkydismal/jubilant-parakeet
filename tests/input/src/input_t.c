#include "input_t.h"

#include "test.h"

// TODO: Implement
TEST( input_t$convert$toStaticString ) {
    // clang-format off
    direction_t l_directions[] = {
        UP, DOWN, LEFT, RIGHT,
    };

    button_t l_buttons[] = {
        A, B, C, D,
    };

    const char l_directionsConverted[] = {
        '8',  // UP
        '2',  // DOWN
        '4',  // LEFT
        '6',  // RIGHT
    };

    const char l_buttonsConverted[] = {
        'A', 'B', 'C', 'D',
    };
    // clang-format on

    ASSERT_STRING_EQ( input_t$convert$toStaticString(
                          MAKE_INPUT( ( UP | DOWN ), ( A | B | C ) ) ),
                      "82ABC" );

    ASSERT_STRING_EQ( input_t$convert$toStaticString(
                          MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) ) ),
                      "46ABD" );

    ASSERT_STRING_EQ(
        input_t$convert$toStaticString( MAKE_INPUT( ( DOWN | RIGHT ), ( A ) ) ),
        "3A" );

    {
        char l_expected[ 3 ];

        l_expected[ 2 ] = '\0';

        FOR_RANGE( size_t, 0, arrayLengthNative( l_directions ) ) {
            const size_t l_directionIndex = _index;

            FOR_RANGE( size_t, 0, arrayLengthNative( l_buttons ) ) {
                const size_t l_buttonIndex = _index;

                l_expected[ 0 ] = l_directionsConverted[ l_directionIndex ];
                l_expected[ 1 ] = l_buttonsConverted[ l_buttonIndex ];

                const input_t l_input =
                    MAKE_INPUT( l_directions[ l_directionIndex ],
                                l_buttons[ l_buttonIndex ] );

                const char* l_actual =
                    input_t$convert$toStaticString( l_input );

                ASSERT_STRING_EQ( l_actual, l_expected );
            }
        }
    }
}

TEST(input_t_convert_toStaticString_all_combinations) {
    // clang-format off
    const direction_t ALL_DIRS = (direction_t)(UP | DOWN | LEFT | RIGHT);
    const button_t    ALL_BUTTONS = (button_t)(A | B | C | D);
    // clang-format on

    // Helper to build expected string for one (dirMask, btnMask).
    auto buildExpected = [](direction_t dirMask, button_t btnMask, char *out) {
        size_t pos = 0;

        // Check for exact two-bit diagonals (and no others):
        // UP+LEFT => '7', UP+RIGHT => '9', DOWN+LEFT => '1', DOWN+RIGHT => '3'
        // Only when exactly those two bits are set, no extra direction bits.
        if ((dirMask & (UP|LEFT)) == (UP|LEFT) && (dirMask & ~(UP|LEFT)) == 0) {
            out[pos++] = '7';
        }
        else if ((dirMask & (UP|RIGHT)) == (UP|RIGHT) && (dirMask & ~(UP|RIGHT)) == 0) {
            out[pos++] = '9';
        }
        else if ((dirMask & (DOWN|LEFT)) == (DOWN|LEFT) && (dirMask & ~(DOWN|LEFT)) == 0) {
            out[pos++] = '1';
        }
        else if ((dirMask & (DOWN|RIGHT)) == (DOWN|RIGHT) && (dirMask & ~(DOWN|RIGHT)) == 0) {
            out[pos++] = '3';
        }
        else {
            // Otherwise list each in fixed order UP, DOWN, LEFT, RIGHT if set.
            if (dirMask & UP)    out[pos++] = '8';
            if (dirMask & DOWN)  out[pos++] = '2';
            if (dirMask & LEFT)  out[pos++] = '4';
            if (dirMask & RIGHT) out[pos++] = '6';
        }

        // Append buttons in fixed order A, B, C, D
        if (btnMask & A) out[pos++] = 'A';
        if (btnMask & B) out[pos++] = 'B';
        if (btnMask & C) out[pos++] = 'C';
        if (btnMask & D) out[pos++] = 'D';

        out[pos] = '\0';
    };

    // Buffer for expected string: worst-case length:
    // directions: up to 4 chars if listing individually (e.g., UP|DOWN|LEFT|RIGHT => "8246"), plus up to 4 buttons => total 8, plus null.
    char l_expected[9];

    // Iterate all subsets of direction bits and button bits:
    for (unsigned dirMask = 0; dirMask <= (unsigned)ALL_DIRS; ++dirMask) {
        if (dirMask & ~((unsigned)ALL_DIRS)) {
            // skip any bits outside defined flags
            continue;
        }
        for (unsigned btnMask = 0; btnMask <= (unsigned)ALL_BUTTONS; ++btnMask) {
            if (btnMask & ~((unsigned)ALL_BUTTONS)) {
                continue;
            }

            direction_t l_dir = (direction_t)dirMask;
            button_t    l_btn = (button_t)btnMask;

            // Build expected string
            buildExpected(l_dir, l_btn, l_expected);

            // Construct input and get actual
            const input_t l_input = MAKE_INPUT(l_dir, l_btn);
            const char* l_actual = input_t$convert$toStaticString(l_input);

            // Compare
            ASSERT_STRING_EQ(l_actual, l_expected);
        }
    }
}
