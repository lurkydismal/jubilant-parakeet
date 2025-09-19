#include "ctre.hpp"

#include "test.hpp"

// Match only digits
using std::string_view;

using digitsMatcher_t = decltype( ctre::match< "^[0-9]+$" > );

// Match lowercase words
using wordMatcher_t = decltype( ctre::match< "^[a-z]+$" > );

// Match hex color code (# followed by 6 hex digits)
using hexcolorMatcher_t = decltype( ctre::match< "^#[0-9A-Fa-f]{6}$" > );

template < typename Matcher >
auto hasMatch( string_view _input ) -> bool {
    return Matcher{}( _input );
}

// Digits
TEST( CtreMatchTest, Digits_Positive ) {
    EXPECT_TRUE( hasMatch< digitsMatcher_t >( "123" ) );
    EXPECT_TRUE( hasMatch< digitsMatcher_t >( "007" ) );
}

TEST( CtreMatchTest, Digits_Negative ) {
    EXPECT_FALSE( hasMatch< digitsMatcher_t >( "12a3" ) );
    EXPECT_FALSE( hasMatch< digitsMatcher_t >( "" ) );
}

// Words
TEST( CtreMatchTest, Word_Positive ) {
    EXPECT_TRUE( hasMatch< wordMatcher_t >( "hello" ) );
    EXPECT_TRUE( hasMatch< wordMatcher_t >( "world" ) );
}

TEST( CtreMatchTest, Word_Negative ) {
    EXPECT_FALSE(
        hasMatch< wordMatcher_t >( "Hello" ) ); // uppercase not allowed
    EXPECT_FALSE( hasMatch< wordMatcher_t >( "hi123" ) );
}

// Hex colors
TEST( CtreMatchTest, HexColor_Positive ) {
    EXPECT_TRUE( hasMatch< hexcolorMatcher_t >( "#FFAACC" ) );
    EXPECT_TRUE( hasMatch< hexcolorMatcher_t >( "#00ff00" ) );
}

TEST( CtreMatchTest, HexColor_Negative ) {
    EXPECT_FALSE( hasMatch< hexcolorMatcher_t >( "#GGGGGG" ) ); // invalid hex
    EXPECT_FALSE( hasMatch< hexcolorMatcher_t >( "FFAACC" ) );  // missing #
}
