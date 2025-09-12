#include "test.hpp"

#include <array>
#include <variant>
#include <vector>

namespace {

template < typename... Arguments >
consteval auto t( Arguments&&... _arguments ) {
    using varying_t = std::variant< std::decay_t< Arguments >... >;

    std::vector< varying_t > a;

    return ( a );
}

auto x = t( true, "", 123 );

} // namespace

namespace test {

TEST( assertTrue, ( [] { assertTrue( true ); } ) );

TEST( assertFalse, ( [] { assertFalse( false ); } ) );

TEST( assertEqual, ( [] { assertEqual( true, true ); } ) );

TEST( assertNotEqual, ( [] { assertNotEqual( true, false ); } ) );

} // namespace test
