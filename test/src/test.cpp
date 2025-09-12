#include "test.hpp"

#if defined( TESTS )

#include <algorithm>
#include <ranges>
#include <span>

#include "stdfunc.hpp"

namespace test {

std::vector< std::pair< std::string, testFunction_t > > g_testRegistry;

} // namespace test

auto main( int _argumentCount, char** _argumentsVector ) -> int {
    size_t l_passed = 0;
    size_t l_failed = 0;

    std::println( "{}--- Running {} tests ---{}", stdfunc::color::g_yellow,
                  test::g_testRegistry.size(), stdfunc::color::g_reset );

    for ( auto [ _name, _function ] :
          test::g_testRegistry | std::views::filter( [ & ]( auto& _pair ) {
              return ( !std::ranges::contains(
                  std::span< char* >( _argumentsVector, _argumentCount ),
                  _pair.first ) );
          } ) ) {
        std::println( "{}Running {} test...{}", stdfunc::color::g_cyanLight,
                      _name, stdfunc::color::g_reset );

        if ( !_function() ) {
            std::println( "{}[PASSED]{}{}", stdfunc::color::g_green,
                          stdfunc::color::g_reset, _name );

            l_passed++;

        } else {
            l_failed++;
        }
    }

    std::println( "{}--- Test Summary ---{}", stdfunc::color::g_yellow,
                  stdfunc::color::g_reset );

    if ( l_passed ) {
        std::println( "{}Passed: {}{}", stdfunc::color::g_green, l_passed,
                      stdfunc::color::g_reset );
    }

    if ( l_failed ) {
        std::println( "{}Failed: {}{}", stdfunc::color::g_red, l_failed,
                      stdfunc::color::g_reset );
    }

    return ( l_failed > 0 );
}

#endif
