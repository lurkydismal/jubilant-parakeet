#pragma once

#if defined( TESTS )

#include <print>
#include <source_location>
#include <string>
#include <utility>
#include <vector>

#include "stdfunc.hpp"

namespace test {

using testFunction_t = int ( * )();

extern std::vector< std::pair< std::string, testFunction_t > > g_testRegistry;

[[gnu::used]] static bool g_hasTestFailed = false;

#define TEST( _name )                                                        \
    static void _name##_test_implementation( void ) [[gnu::used]];           \
    static int _name##_test( void ) {                                        \
        g_status = false;                                                    \
        _name##_test_implementation();                                       \
        return ( g_status );                                                 \
    }                                                                        \
    [[gnu::constructor, used]] static void register_##_name##_test( void ) { \
        g_testRegistry[ g_testCount++ ] =                                    \
            ( testEntry_t ){ #_name, _name##_test };                         \
    }                                                                        \
    static void _name##_test_implementation( void )

#endif

namespace {

inline void printFailed( std::string_view _reason,
                         std::source_location _sourceLocation ) {
    std::println( "{}[FAILED]{} {}:{}: {}", stdfunc::color::g_red,
                  stdfunc::color::g_reset, _sourceLocation.file_name(),
                  _sourceLocation.line(), _reason );
}

} // namespace

template < typename Checker >
    requires stdfunc::is_lambda< Checker, bool >
constexpr void assert( std::string_view _reason,
                       Checker&& _checker,
                       const std::source_location& _sourceLocation =
                           std::source_location::current() ) {
    if ( g_hasTestFailed ) {
        return;
    }

    if ( !std::forward< Checker >( _checker ) ) {
        printFailed( _reason, _sourceLocation );

        g_hasTestFailed = true;
    }
}

template < typename T >
constexpr void assertTrue( T& _actual ) {
    assert( std::format( "Not true but {}", _actual ),
            [ & ] -> bool { return ( _actual ); } );
}

template < typename T >
constexpr void assertFalse( T& _actual ) {
    assert( std::format( "Not false but {}", _actual ),
            [ & ] -> bool { return ( !_actual ); } );
}

template < typename T >
constexpr void assertEqual( T& _actual, T& _expected ) {
    assert( std::format( "Expected {} but got {}", _expected, _actual ),
            [ & ] -> bool { return ( _actual == _expected ); } );
}

template < typename T >
constexpr void assertNotEqual( T& _actual, T& _expected ) {
    assert( std::format( "Expected different from {} but got {}", _expected,
                         _actual ),
            [ & ] -> bool { return ( _actual != _expected ); } );
}
}
