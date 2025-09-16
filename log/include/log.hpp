#pragma once

#include <iostream>
#include <print>
#include <source_location>
#include <string_view>
#include <thread>

#include "stdfunc.hpp"

namespace {

constexpr std::string_view g_colorThreadId = stdfunc::color::g_purpleLight;
constexpr std::string_view g_colorFileName = stdfunc::color::g_purpleLight;
constexpr std::string_view g_colorLineNumber = stdfunc::color::g_purpleLight;
constexpr std::string_view g_colorFunctionName = stdfunc::color::g_purpleLight;

constexpr auto formatWithColor( auto _what, std::string_view _color )
    -> std::string {
    return ( std::format( "{}{}{}", _color, _what,
                          stdfunc::color::g_resetForeground ) );
}

inline auto formatLocation( const std::source_location& _sourceLocation =
                                std::source_location::current() )
    -> std::string {
    return ( std::format(
        "Thread {}: '{}:{}' "
        "in '{}'",
        formatWithColor( std::this_thread::get_id(), g_colorThreadId ),
        formatWithColor( std::filesystem::path( _sourceLocation.file_name() )
                             .filename()
                             .string(),
                         g_colorFileName ),
        formatWithColor( _sourceLocation.line(), g_colorLineNumber ),
        formatWithColor( _sourceLocation.function_name(),
                         g_colorFunctionName ) ) );
}

#if defined( DEBUG )

template < typename... Arguments >
void _debug( [[maybe_unused]] std::format_string< Arguments... > _format,
             [[maybe_unused]] const std::source_location& _sourceLocation,
             [[maybe_unused]] Arguments&&... _arguments ) {
    std::print( "{}{} | Message: ",
                formatWithColor( "DEBUG: ", stdfunc::color::g_cyanLight ),
                formatLocation( _sourceLocation ) );

    std::print( _format, std::forward< Arguments >( _arguments )... );

    std::println( "{}", stdfunc::color::g_reset );
}

#endif

template < typename... Arguments >
void _error( std::format_string< Arguments... > _format,
             const std::source_location& _sourceLocation,
             Arguments&&... _arguments ) {
    std::print( std::cerr, "{}{} | Message: ",
                formatWithColor( "ERROR: ", stdfunc::color::g_red ),
                formatLocation( _sourceLocation ) );

    std::println( std::cerr, _format,
                  std::forward< Arguments >( _arguments )... );

    std::println( "{}", stdfunc::color::g_reset );
}

} // namespace

namespace logg {

#if defined( DEBUG )

template < typename... Arguments >
void debug( [[maybe_unused]] std::format_string< Arguments... > _format,
            [[maybe_unused]] Arguments&&... _arguments ) {
    _debug( _format, std::source_location::current(),
            std::forward< Arguments >( _arguments )... );
}

template < typename T >
    requires( !std::is_pointer_v< T > )
void _variable( std::string_view _variableName,
                const T& _variable,
                const std::source_location& _sourceLocation =
                    std::source_location::current() ) {
    _debug( "{} = '{}'", _sourceLocation, _variableName, _variable );
}

template < typename T >
    requires( std::is_pointer_v< T > )
void _variable( std::string_view _variableName,
                const T _variable,
                const std::source_location& _sourceLocation =
                    std::source_location::current() ) {
    _debug( "{} = '0x{:08x}'", _sourceLocation, _variableName,
            std::bit_cast< uintptr_t >( _variable ) );
}

#define variable( _variableToLog ) _variable( #_variableToLog, _variableToLog )

#else

template < typename... Arguments >
void debug( [[maybe_unused]] std::format_string< Arguments... > _format,
            [[maybe_unused]] Arguments&&... _arguments ) {}

#define variable( _variableToLog ) ( ( void )_variableToLog )

#endif

template < typename... Arguments >
void info( std::format_string< Arguments... > _format,
           Arguments&&... _arguments ) {
    std::print( "{}", formatWithColor( "INFO: ", stdfunc::color::g_green ) );

    std::println( _format, std::forward< Arguments >( _arguments )... );
}

template < typename... Arguments >
void warning( std::format_string< Arguments... > _format,
              Arguments&&... _arguments ) {
    std::print( std::cerr, "{}",
                formatWithColor( "WARNING: ", stdfunc::color::g_yellow ) );

    std::println( _format, std::forward< Arguments >( _arguments )... );
}

template < typename... Arguments >
void error( std::format_string< Arguments... > _format,
            Arguments&&... _arguments ) {
    _error( _format, std::source_location::current(),
            std::forward< Arguments >( _arguments )... );
}

} // namespace logg
