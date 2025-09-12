#pragma once

#if defined( TESTS )

#include <functional>
#include <print>
#include <source_location>
#include <string>
#include <utility>
#include <vector>

#include "stdfunc.hpp"

namespace test {

using testFunction_t = std::function< bool() >;

extern std::vector< std::pair< std::string, testFunction_t > > g_testRegistry;

[[gnu::used]] static bool g_hasTestFailed = false;

template < typename Lambda >
    requires stdfunc::is_lambda< Lambda, void >
void add( std::string_view _name, Lambda&& _lambda ) {
    g_testRegistry.emplace_back( std::pair{ _name, [ & ] -> bool {
                                               g_hasTestFailed = false;

                                               std::forward< Lambda >(
                                                   _lambda )();

                                               return ( g_hasTestFailed );
                                           } } );
}

#define TEST( _name, _lambda )                                        \
    CONSTRUCTOR [[gnu::used]] static void register_##_name##_test() { \
        test::add( ( #_name ), ( _lambda ) );                         \
    }

template < typename Checker >
    requires stdfunc::is_lambda< Checker, bool >
constexpr void assert( std::string_view _reason,
                       Checker&& _checker,
                       size_t _line,
                       const std::source_location& _sourceLocation =
                           std::source_location::current() ) {
    if ( g_hasTestFailed ) {
        return;
    }

    if ( !std::forward< Checker >( _checker )() ) {
        std::println( "{}[FAILED]{} {}:{}: {}", stdfunc::color::g_red,
                      stdfunc::color::g_reset, _sourceLocation.file_name(),
                      _line, _reason );

        g_hasTestFailed = true;
    }
}

template < typename T >
    requires std::is_convertible_v< T, bool >
constexpr void _assertTrue( T&& _actual, size_t _line ) {
    assert(
        std::format( "Not true but {}", _actual ),
        [ & ] -> bool { return ( std::forward< T >( _actual ) ); }, _line );
}

#define assertTrue( _actual ) test::_assertTrue( ( _actual ), __LINE__ )

template < typename T >
    requires std::is_convertible_v< T, bool >
constexpr void _assertFalse( T _actual, size_t _line ) {
    assert(
        std::format( "Not false but {}", _actual ),
        [ & ] -> bool { return ( !_actual ); }, _line );
}

#define assertFalse( _actual ) test::_assertFalse( ( _actual ), __LINE__ )

template < typename T1, typename T2 >
    requires std::equality_comparable_with< T1, T2 >
constexpr void _assertEqual( T1 _actual, T2 _expected, size_t _line ) {
    assert(
        std::format( "Expected {} but got {}", _expected, _actual ),
        [ & ] -> bool { return ( _actual == _expected ); }, _line );
}

#define assertEqual( _actual, _expected ) \
    test::_assertEqual( ( _actual ), ( _expected ), __LINE__ )

template < typename T1, typename T2 >
    requires std::equality_comparable_with< T1, T2 >
constexpr void _assertNotEqual( T1 _actual, T2 _expected, size_t _line ) {
    assert(
        std::format( "Expected different from {} but got {}", _expected,
                     _actual ),
        [ & ] -> bool { return ( _actual != _expected ); }, _line );
}

#define assertNotEqual( _actual, _expected ) \
    test::_assertNotEqual( ( _actual ), ( _expected ), __LINE__ )

} // namespace test

#endif
