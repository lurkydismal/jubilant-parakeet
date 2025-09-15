#pragma once

#include <ctll.hpp>
#include <ctre.hpp>
#include <xxhash.h>

#include <algorithm>
#include <cstdarg>
#include <filesystem>
#include <gsl/pointers>
#include <random>
#include <ranges>
#include <regex>
#include <string_view>
#include <type_traits>

#if ( defined( DEBUG ) && !defined( TESTS ) )

#include <iostream>
#include <print>
#include <source_location>
#include <stacktrace>
#include <thread>

#endif

// Function attributes
#define FORCE_INLINE [[gnu::always_inline]] inline
#define NO_OPTIMIZE [[gnu::optimize( "0" )]]
#define CONST [[gnu::const]]
#define PURE [[gnu::pure]]
#define HOT [[gnu::hot]]
#define COLD [[gnu::cold]]
#define SENTINEL [[gnu::sentinel]]
#define CONSTRUCTOR [[gnu::constructor]]
#define DESTRUCTOR [[gnu::destructor]]
#define EXPORT extern "C"

// Struct attributes
#define PACKED [[gnu::packed]]

namespace stdfunc {

// Constants
inline constexpr char g_commentSymbol = '#';
inline constexpr size_t g_decimalRadix = 10;

namespace color {

inline constexpr std::string_view g_cyanLight = "\x1b[1;36m";
inline constexpr std::string_view g_blueLight = "\x1b[1;34m";
inline constexpr std::string_view g_green = "\x1b[1;32m";
inline constexpr std::string_view g_purpleLight = "\x1b[1;35m";
inline constexpr std::string_view g_red = "\x1b[1;31m";
inline constexpr std::string_view g_yellow = "\x1b[1;33m";
inline constexpr std::string_view g_resetForeground = "\x1b[39m";
inline constexpr std::string_view g_resetBackground = "\x1b[49m";
inline constexpr std::string_view g_reset = "\x1b[0m";

} // namespace color

// Concepts
template < typename... Arguments >
concept has_common_type = ( requires {
    typename std::common_type_t< Arguments... >;
} && ( !std::is_void_v< std::common_type_t< Arguments... > > ));

template < typename T >
concept is_container = ( std::ranges::range< T > && requires( T _container ) {
    typename T::value_type;
    { _container.size() } -> std::convertible_to< std::size_t >;
} );

template < typename T >
concept is_struct = ( std::is_class_v< T > && !std::is_union_v< T > );

template < typename Lambda, typename ReturnType, typename... Arguments >
concept is_lambda =
    ( std::invocable< Lambda, Arguments... > &&
      std::convertible_to< std::invoke_result_t< Lambda, Arguments... >,
                           ReturnType > );

// Utility macros ( no side-effects )
#define STRINGIFY( _value ) #_value

// Debug utility functions ( side-effects )

#if defined( assert )

#undef assert

#endif

#if ( defined( DEBUG ) && !defined( TESTS ) )

namespace {

constexpr std::string_view g_trapColorLevel = color::g_red;
constexpr std::string_view g_trapColorThreadId = color::g_purpleLight;
constexpr std::string_view g_trapColorFileName = color::g_purpleLight;
constexpr std::string_view g_trapColorLineNumber = color::g_purpleLight;
constexpr std::string_view g_trapColorFunctionName = color::g_purpleLight;

constexpr size_t g_backtraceLimit = 5;

template < typename... Arguments >
[[noreturn]] void _trap( std::format_string< Arguments... > _format,
                         const std::source_location _sourceLocation,
                         Arguments&&... _arguments ) {
    std::print( std::cerr,
                "{}[TRAP] {}Thread {}{}{}: File '{}{}{}': line {}{}{} "
                "in function '{}{}{}' | Message: ",
                g_trapColorLevel, color::g_resetForeground, g_trapColorThreadId,
                std::this_thread::get_id(), color::g_resetForeground,
                g_trapColorFileName, _sourceLocation.file_name(),
                color::g_resetForeground, g_trapColorLineNumber,
                _sourceLocation.line(), color::g_resetForeground,
                g_trapColorFunctionName, _sourceLocation.function_name(),
                color::g_resetForeground );
    std::println( std::cerr, _format,
                  std::forward< Arguments >( _arguments )... );
    std::println( "{}", std::stacktrace::current( 2, g_backtraceLimit ) );

    __builtin_trap();
}

} // namespace

template < typename... Arguments >
[[noreturn]] void trap( std::format_string< Arguments... > _format = "",
                        Arguments&&... _arguments ) {
    _trap( _format, std::source_location::current(),
           std::forward< Arguments >( _arguments )... );
}

template < typename... Arguments >
constexpr void assert( bool _result,
                       std::format_string< Arguments... > _format = "",
                       Arguments&&... _arguments ) {
    if ( !_result ) [[unlikely]] {
        _trap( _format, std::source_location::current(),
               std::forward< Arguments >( _arguments )... );
    }
}

#else

template < typename... Arguments >
void trap( [[maybe_unused]] std::format_string< Arguments... > _format = "",
           [[maybe_unused]] Arguments&&... _arguments ) {}

template < typename... Arguments >
constexpr void assert(
    [[maybe_unused]] bool _result,
    [[maybe_unused]] std::format_string< Arguments... > _format = "",
    [[maybe_unused]] Arguments&&... _arguments ) {}

#endif

// Literals ( no side-effects )
[[nodiscard]] constexpr auto operator""_b( char _symbol ) -> std::byte {
    return ( static_cast< std::byte >( _symbol ) );
}

[[nodiscard]] constexpr auto operator""_b( unsigned long long _symbol )
    -> std::byte {
    assert( _symbol <= 0xFF );

    return ( static_cast< std::byte >( _symbol ) );
}

template < typename SymbolTypes, SymbolTypes... _symbols >
[[nodiscard]] constexpr auto operator""_bytes() {
    assert( ( ... && ( _symbols <= 0xFF ) ) );

    return ( std::array< std::byte, sizeof...( _symbols ) >{
        std::byte{ _symbols }... } );
}

// Utility functions ( no side-effects )
template < typename T >
    requires std::is_arithmetic_v< T >
[[nodiscard]] constexpr auto bitsToBytes( T _bits ) -> T {
    return ( ( _bits + 7 ) / 8 );
}

template < typename T >
    requires std::is_arithmetic_v< T >
[[nodiscard]] constexpr auto lengthOfNumber( T _number ) -> size_t {
    return ( ( _number < 10ULL )                     ? ( 1 )
             : ( _number < 100ULL )                  ? ( 2 )
             : ( _number < 1000ULL )                 ? ( 3 )
             : ( _number < 10000ULL )                ? ( 4 )
             : ( _number < 100000ULL )               ? ( 5 )
             : ( _number < 1000000ULL )              ? ( 6 )
             : ( _number < 10000000ULL )             ? ( 7 )
             : ( _number < 100000000ULL )            ? ( 8 )
             : ( _number < 1000000000ULL )           ? ( 9 )
             : ( _number < 10000000000ULL )          ? ( 10 )
             : ( _number < 100000000000ULL )         ? ( 11 )
             : ( _number < 1000000000000ULL )        ? ( 12 )
             : ( _number < 10000000000000ULL )       ? ( 13 )
             : ( _number < 100000000000000ULL )      ? ( 14 )
             : ( _number < 1000000000000000ULL )     ? ( 15 )
             : ( _number < 10000000000000000ULL )    ? ( 16 )
             : ( _number < 100000000000000000ULL )   ? ( 17 )
             : ( _number < 1000000000000000000ULL )  ? ( 18 )
             : ( _number < 10000000000000000000ULL ) ? ( 19 )
                                                     : ( 20 ) );
}

[[nodiscard]] constexpr auto isSpace( char _symbol ) -> bool {
    return ( ( _symbol == ' ' ) || ( _symbol == '\f' ) || ( _symbol == '\n' ) ||
             ( _symbol == '\r' ) || ( _symbol == '\t' ) ||
             ( _symbol == '\v' ) );
}

[[nodiscard]] constexpr auto sanitizeString( std::string_view _string ) {
    return ( _string | std::views::drop_while( isSpace ) |
             std::views::take_while( []( char _symbol ) {
                 return ( _symbol != g_commentSymbol );
             } ) |
             std::views::reverse | std::views::drop_while( isSpace ) |
             std::views::reverse );
}

// Utility functions ( side-effects )
namespace random {

using engine_t = std::
    conditional_t< ( sizeof( size_t ) > 4 ), std::mt19937_64, std::mt19937 >;

extern thread_local engine_t g_engine;

template < typename T >
    requires std::is_arithmetic_v< T >
auto number( T _min, T _max ) -> T {
    using distribution_t =
        std::conditional_t< std::is_integral_v< T >,
                            std::uniform_int_distribution< T >,
                            std::uniform_real_distribution< T > >;

    return ( ( distribution_t( _min, _max ) )( g_engine ) );
}

template < typename T >
    requires std::is_arithmetic_v< T >
auto number() -> T {
    using numericLimit_t = std::numeric_limits< T >;

    const auto l_max = numericLimit_t::max();

    if constexpr ( std::is_integral_v< T > ) {
        return ( ( std::uniform_int_distribution< T >( numericLimit_t::min(),
                                                       l_max ) )( g_engine ) );

    } else if constexpr ( std::is_floating_point_v< T > ) {
        return ( ( std::uniform_real_distribution< T >(
            numericLimit_t::lowest(), l_max ) )( g_engine ) );
    }
}

template < typename Container >
    requires is_container< Container >
auto value( const Container& _container ) -> typename Container::value_type& {
    assert( !_container.empty() );

    return ( _container.at( number< size_t >( 0, _container.size() - 1 ) ) );
}

template < typename Container >
    requires is_container< Container >
auto view( const Container& _container ) {
    assert( !_container.empty() );

    return ( std::views::iota( 0 ) | std::views::transform( [ & ]( auto ) {
                 return ( randomValueFromContainer( _container ) );
             } ) );
}

template < typename Container, typename T = typename Container::value_type >
    requires is_container< Container > && std::is_arithmetic_v< T >
void fill( Container& _container, T _min, T _max ) {
    std::ranges::generate( _container,
                           [ & ] { return ( number< T >( _min, _max ) ); } );
}

template < typename Container, typename T = typename Container::value_type >
    requires is_container< Container > && std::is_same_v< T, std::byte >
void fill( Container& _container, uint8_t _min, uint8_t _max ) {
    std::ranges::generate( _container, [ & ] {
        return ( static_cast< std::byte >( number< uint8_t >( _min, _max ) ) );
    } );
}

template < typename Container, typename T = typename Container::value_type >
    requires is_container< Container > && std::is_arithmetic_v< T >
void fill( Container& _container ) {
    std::ranges::generate( _container, [ & ] { return ( number< T >() ); } );
}

template < typename Container, typename T = typename Container::value_type >
    requires is_container< Container > && std::is_same_v< T, std::byte >
void fill( Container& _container ) {
    std::ranges::generate( _container, [ & ] {
        return ( static_cast< std::byte >( number< uint8_t >() ) );
    } );
}

} // namespace random

[[nodiscard]] constexpr auto generateHash( std::span< std::byte > _data,
                                           size_t _seed = 0x9e3779b1 )
    -> size_t {
    size_t l_returnValue = XXH32( _data.data(), _data.size(), _seed );

    return ( l_returnValue );
}

template < template < typename > typename Container, typename... Arguments >
[[nodiscard]] constexpr auto makeVariantContainer( Arguments&&... _arguments ) {
    using variant_t = std::variant< std::decay_t< Arguments >... >;

    return ( Container< variant_t >{
        variant_t( std::forward< Arguments >( _arguments ) )... } );
}

template < template < typename, size_t > typename Container,
           typename... Arguments >
[[nodiscard]] constexpr auto makeVariantContainer( Arguments&&... _arguments ) {
    using variant_t = std::variant< std::decay_t< Arguments >... >;

    return ( std::array< variant_t, sizeof...( Arguments ) >{
        variant_t( std::forward< Arguments >( _arguments ) )... } );
}

namespace filesystem {

// Utility OS specific functions ( no side-effects )
[[nodiscard]] inline auto getApplicationDirectoryAbsolutePath()
    -> std::optional< std::filesystem::path > {
    std::optional< std::filesystem::path > l_returnValue = std::nullopt;

    do {
        std::array< char, PATH_MAX > l_executablePath{};

        // Get executable path
        {
            const ssize_t l_executablePathLength = readlink(
                "/proc/self/exe", l_executablePath.data(), ( PATH_MAX - 1 ) );

            if ( l_executablePathLength == -1 ) [[unlikely]] {
                break;
            }
        }

        l_returnValue =
            std::filesystem::path( std::string_view( l_executablePath ) )
                .remove_filename();
    } while ( false );

    return ( l_returnValue );
}

namespace {

template < typename Matcher >
    requires is_lambda< Matcher, bool, const std::string& >
[[nodiscard]] auto _getPathsByRegexp( std::string_view _directory,
                                      Matcher _matcher )
    -> std::vector< std::filesystem::path > {
    std::vector< std::filesystem::path > l_returnValue;

    for ( const auto& _entry :
          std::filesystem::directory_iterator( _directory ) ) {
        if ( !_entry.is_regular_file() ) {
            continue;
        }

        const std::string l_filename = _entry.path().filename().string();

        if ( _matcher( l_filename ) ) {
            l_returnValue.emplace_back( _entry.path() );
        }
    }

    return ( l_returnValue );
}

} // namespace

// Runtime regexp
[[nodiscard]] inline auto getPathsByRegexp( std::string& _regexp,
                                            std::string_view _directory )
    -> std::vector< std::filesystem::path > {
    std::regex l_matcher( _regexp );

    return (
        _getPathsByRegexp( _directory, [ & ]( const std::string& _fileName ) {
            return ( std::regex_match( _fileName, l_matcher ) );
        } ) );
}

// Compile-time regexp
template < size_t N >
    requires( N > 0 )
[[nodiscard]] auto getPathsByRegexp( const ctll::fixed_string< N >& _regexp,
                                     std::string_view _directory )
    -> std::vector< std::filesystem::path > {
    auto l_matcher = ctre::match< _regexp >;

    return (
        _getPathsByRegexp( _directory, [ & ]( const std::string& _fileName ) {
            return ( l_matcher( _fileName ) );
        } ) );
}

} // namespace filesystem

namespace compress {

/**
 * @brief [TODO:description]
 *
 * @detailed Compression level: Level 1 is the fastest Level 2 is a little
 * slower but provides better compression.
 *
 * @param _data String view
 * @param _level Compression level
 *
 * @return Compressed string
 */
// TODO: Make constexpr
[[nodiscard]] auto text( std::string_view _text, size_t _level = 1 )
    -> std::optional< std::string >;

/**
 * @brief [TODO:description]
 *
 * @detailed Compression level: TODO: Write
 *
 * @param _data Data view
 * @param _level Compression level
 *
 * @return Compressed data
 */
// TODO: Make constexpr
[[nodiscard]] auto data( std::span< std::byte > _data, size_t _level = 3 )
    -> std::optional< std::vector< std::byte > >;

} // namespace compress

namespace decompress {

/**
 * @brief [TODO:description]
 *
 * @param _data Data view
 *
 * @return Decompressed string
 */
// TODO: Make constexpr
[[nodiscard]] auto text( std::string_view _data )
    -> std::optional< std::string >;

/**
 * @brief [TODO:description]
 *
 * @param _data Data view
 *
 * @return Decompressed string
 */
// TODO: Make constexpr
[[nodiscard]] auto data( std::span< std::byte > _data, size_t _originalSize )
    -> std::optional< std::vector< std::byte > >;

} // namespace decompress

namespace meta {

// Utility Compiler specific functions ( side-effects )
// format - "%s%s %s = %p'
SENTINEL
inline void dumpCallback( void* _callback,
                          void* _context,
                          const char* _format,
                          ... )
    __attribute__( ( format( printf,
                             3, // Format index
                             4  // First format argument index
                             ) ) ) {
    thread_local static size_t l_depth = 0;

    va_list l_arguments;

    va_start( l_arguments, _format );

    do {
        std::string_view l_format = _format;

        // Skip nested structs
        if ( l_format.contains( '{' ) ) {
            l_depth++;
        }

        // Only act on top-level fields
        if ( ( l_depth == 1 ) && ( l_format.contains( '=' ) ) ) {
            // Skip indentation
            va_arg( l_arguments, char* );

            // Skip type
            va_arg( l_arguments, char* );

            // Field name
            std::string_view l_fieldName = va_arg( l_arguments, char* );

            using callback_t =
                bool ( * )( std::string_view _fieldName, void* _context );

            const auto l_callback = std::bit_cast< callback_t >( _callback );

            bool l_result = l_callback( l_fieldName, _context );

            if ( !l_result ) [[unlikely]] {
                break;
            }
        }

        if ( l_format.contains( '}' ) ) {
            l_depth--;
        }
    } while ( false );

    va_end( l_arguments );
}

template < typename T >
    requires is_struct< T >
void iterateStructUnionTopMostFields( gsl::not_null< void* > _callback,
                                      gsl::not_null< void* > _context ) {
    T l_structSample;

    __builtin_dump_struct( &l_structSample, dumpCallback, _callback, _context );
}

} // namespace meta

} // namespace stdfunc
