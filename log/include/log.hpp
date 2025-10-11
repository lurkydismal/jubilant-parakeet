#pragma once

#include <iostream>
#include <print>
#include <ranges>
#include <source_location>
#include <string_view>
#include <thread>

#include "stdfunc.hpp"

namespace {

constexpr std::string_view g_colorThreadId = stdfunc::color::g_purpleLight;
constexpr std::string_view g_colorFileName = stdfunc::color::g_purpleLight;
constexpr std::string_view g_colorLineNumber = stdfunc::color::g_purpleLight;
constexpr std::string_view g_colorFunctionName = stdfunc::color::g_purpleLight;

template < typename T >
constexpr auto formatWithColor( T&& _what, std::string_view _color )
    -> std::string {
    return ( std::format( "{}{}{}", _color, std::forward< T >( _what ),
                          stdfunc::color::g_resetForeground ) );
}

inline auto formatLocation( const std::source_location& _sourceLocation =
                                std::source_location::current() )
    -> std::string {
    std::string_view l_functionNameMangled = _sourceLocation.function_name();

    std::string l_functionName =
        l_functionNameMangled |
        std::views::drop_while( []( char _symbol ) -> bool {
            return ( !stdfunc::isSpace( _symbol ) );
        } ) |
        std::views::drop( 1 ) |
        std::views::take_while(
            []( char _symbol ) -> bool { return ( _symbol != '(' ); } ) |
        std::ranges::to< std::string >();

    return ( std::format(
        "Thread {}{:#X}{}: '{}:{}' "
        "in '{}'",
        g_colorThreadId,
        std::hash< std::thread::id >{}( std::this_thread::get_id() ),
        stdfunc::color::g_resetForeground,
        formatWithColor( std::filesystem::path( _sourceLocation.file_name() )
                             .filename()
                             .string(),
                         g_colorFileName ),
        formatWithColor( _sourceLocation.line(), g_colorLineNumber ),
        formatWithColor( l_functionName, g_colorFunctionName ) ) );
}

} // namespace

namespace logg {

#if defined( DEBUG )

[[maybe_unused]] inline void _debug(
    std::string_view _message,
    const std::source_location& _sourceLocation =
        std::source_location::current() ) {
    std::println( "{}{} | Message: {}{}",
                  formatWithColor( "DEBUG: ", stdfunc::color::g_cyanLight ),
                  formatLocation( _sourceLocation ), _message,
                  stdfunc::color::g_reset );
}

#define debug( _format, ... ) _debug( std::format( _format, ##__VA_ARGS__ ) )

template < typename T >
    requires( !std::is_pointer_v< T > )
void _variable( std::string_view _variableName,
                const T& _variable,
                const std::source_location& _sourceLocation =
                    std::source_location::current() ) {
    _debug( std::format( "{} = '{}'", _variableName, _variable ),
            _sourceLocation );
}

template < typename T >
    requires( std::is_pointer_v< T > )
void _variable( std::string_view _variableName,
                const T _variable,
                const std::source_location& _sourceLocation =
                    std::source_location::current() ) {
    _debug( std::format( "{} = '0x{:08x}'", _variableName,
                         std::bit_cast< uintptr_t >( _variable ) ),
            _sourceLocation );
}

#define variable( _variableToLog ) _variable( #_variableToLog, _variableToLog )

#else

template < stdfunc::is_formattable... Arguments >
void debug( [[maybe_unused]] std::format_string< Arguments... > _format,
            [[maybe_unused]] Arguments&&... _arguments ) {}

void variable( [[maybe_unused]] auto&& _variableToLog ) {}

#endif

template < stdfunc::is_formattable... Arguments >
void info( std::format_string< Arguments... > _format,
           Arguments&&... _arguments ) {
    std::print( "{}", formatWithColor( "INFO: ", stdfunc::color::g_green ) );

    std::println( _format, std::forward< Arguments >( _arguments )... );
}

template < stdfunc::is_formattable... Arguments >
void warning( std::format_string< Arguments... > _format,
              Arguments&&... _arguments ) {
    std::print( std::cerr, "{}",
                formatWithColor( "WARNING: ", stdfunc::color::g_yellow ) );

    std::println( std::cerr, _format,
                  std::forward< Arguments >( _arguments )... );
}

[[maybe_unused]] inline void _error(
    std::string_view _message,
    const std::source_location& _sourceLocation =
        std::source_location::current() ) {
    std::println( std::cerr, "{}{} | Message: {}{}",
                  formatWithColor( "ERROR: ", stdfunc::color::g_red ),
                  formatLocation( _sourceLocation ), _message,
                  stdfunc::color::g_reset );
}

#define error( _format, ... ) _error( std::format( _format, ##__VA_ARGS__ ) )

} // namespace logg
