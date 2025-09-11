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

inline auto formatPrefix( std::string_view _prefix,
                          std::string_view _prefixColor = "" ) -> std::string {
    return ( std::format( "{}{}{}", _prefixColor, _prefix,
                          stdfunc::color::g_resetForeground ) );
}

inline auto formatLocation( std::string_view _prefix,
                            std::string_view _prefixColor = "",
                            const std::source_location& _sourceLocation =
                                std::source_location::current() )
    -> std::string {
    return ( std::format(
        "{}Thread {}{}{}: File '{}{}{}': line {}{}{} "
        "in function '{}{}{}' | Message: ",
        formatPrefix( _prefix, _prefixColor ),
        stdfunc::color::g_resetForeground, g_colorThreadId,
        std::this_thread::get_id(), stdfunc::color::g_resetForeground,
        g_colorFileName, _sourceLocation.file_name(),
        stdfunc::color::g_resetForeground, g_colorLineNumber,
        _sourceLocation.line(), stdfunc::color::g_resetForeground,
        g_colorFunctionName, _sourceLocation.function_name(),
        stdfunc::color::g_resetForeground ) );
}

#if defined( DEBUG )

template < typename... Arguments >
inline void _debug(
    [[maybe_unused]] std::format_string< Arguments... > _format,
    [[maybe_unused]] const std::source_location& _sourceLocation,
    [[maybe_unused]] Arguments&&... _arguments ) {
    std::print( "{}", formatLocation( "DEBUG: ", stdfunc::color::g_cyanLight,
                                      _sourceLocation ) );

    std::println( _format, std::forward< Arguments >( _arguments )... );
}

#endif

template < typename... Arguments >
inline void _error( std::format_string< Arguments... > _format,
                    const std::source_location& _sourceLocation,
                    Arguments&&... _arguments ) {
    std::print(
        std::cerr, "{}",
        formatLocation( "ERROR: ", stdfunc::color::g_red, _sourceLocation ) );

    std::println( std::cerr, _format,
                  std::forward< Arguments >( _arguments )... );
}

} // namespace

namespace logg {

#if defined( DEBUG )

template < typename... Arguments >
inline void debug( [[maybe_unused]] std::format_string< Arguments... > _format,
                   [[maybe_unused]] Arguments&&... _arguments ) {
    _debug( _format, std::source_location::current(),
            std::forward< Arguments >( _arguments )... );
}

template < typename T >
    requires( !std::is_pointer_v< T > )
inline void _variable( std::string_view _variableName,
                       const T& _variable,
                       const std::source_location& _sourceLocation =
                           std::source_location::current() ) {
    _debug( "{} = '{}'", _sourceLocation, _variableName, _variable );
}

template < typename T >
    requires( std::is_pointer_v< T > )
inline void _variable( std::string_view _variableName,
                       const T _variable,
                       const std::source_location& _sourceLocation =
                           std::source_location::current() ) {
    _debug( "{} = '0x{:08x}'", _sourceLocation, _variableName,
            std::bit_cast< uintptr_t >( _variable ) );
}

#define variable( _variableToLog ) _variable( #_variableToLog, _variableToLog )

#else

template < typename... Arguments >
inline void debug( [[maybe_unused]] std::format_string< Arguments... > _format,
                   [[maybe_unused]] Arguments&&... _arguments ) {}

#define variable( _variableToLog ) ( ( void )_variableToLog )

#endif

template < typename... Arguments >
inline void info( std::format_string< Arguments... > _format,
                  Arguments&&... _arguments ) {
    std::print( "{}", formatPrefix( "INFO: ", stdfunc::color::g_green ) );

    std::println( _format, std::forward< Arguments >( _arguments )... );
}

template < typename... Arguments >
inline void warning( std::format_string< Arguments... > _format,
                     Arguments&&... _arguments ) {
    std::print( std::cerr, "{}",
                formatPrefix( "WARNING: ", stdfunc::color::g_yellow ) );

    std::println( _format, std::forward< Arguments >( _arguments )... );
}

template < typename... Arguments >
inline void error( std::format_string< Arguments... > _format,
                   Arguments&&... _arguments ) {
    _error( _format, std::source_location::current(),
            std::forward< Arguments >( _arguments )... );
}

} // namespace logg
