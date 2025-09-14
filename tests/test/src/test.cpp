#include "test.hpp"

#include "stdfunc.hpp"

namespace {

auto g_array1 =
    stdfunc::makeVariantContainer< std::array >( true,
                                                 std::string_view( " true " ),
                                                 123,
                                                 "",
                                                 'a' );
auto g_array2 =
    stdfunc::makeVariantContainer< std::array >( false,
                                                 std::string_view( " false " ),
                                                 321,
                                                 " ",
                                                 'b' );

} // namespace

TEST( test, EXPECT_TRUE ) {
    EXPECT_TRUE( true );
    EXPECT_TRUE( 1 );
    EXPECT_TRUE( !0 );
    EXPECT_TRUE( "" );
}

TEST( test, EXPECT_FALSE ) {
    EXPECT_FALSE( false );
    EXPECT_FALSE( 0 );
    EXPECT_FALSE( int{ -1 } );
    EXPECT_FALSE( !1 );
}

TEST( test, EXPECT_EQ ) {
    for ( const auto& _element : g_array1 ) {
        std::visit( []( auto&& _value ) { EXPECT_EQ( _value, _value ); },
                    _element );
    }
}

TEST( test, EXPECT_NE ) {
    for ( const auto& [ _element1, _element2 ] :
          std::views::zip( g_array1, g_array2 ) ) {
        std::visit(
            []( auto&& _value1, auto&& _value2 ) {
                if constexpr ( std::equality_comparable_with<
                                   decltype( _value1 ),
                                   decltype( _value2 ) > ) {
                    EXPECT_NE( _value1, _value2 );
                }
            },
            _element1, _element2 );
    }
}
