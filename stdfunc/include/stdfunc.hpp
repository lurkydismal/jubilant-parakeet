#pragma once

#include <xxhash.h>

#include <cstdarg>
#include <filesystem>
#include <gsl/pointers>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <source_location>
#include <string_view>

namespace stdfunc {

// Function attributes
#define FORCE_INLINE __attribute__( ( always_inline ) ) inline
#define NO_OPTIMIZE __attribute__( ( optimize( "0" ) ) )
#define DEPRECATED( ... ) \
    __attribute__( ( deprecated __VA_OPT__( ( __VA_ARGS__ ) ) ) )
#define NO_RETURN __attribute__( ( noreturn ) )
#define CONST __attribute__( ( const ) )
#define PURE __attribute__( ( pure ) )
#define HOT __attribute__( ( hot ) )
#define COLD __attribute__( ( cold ) )
#define SENTINEL __attribute__( ( sentinel ) )

// Struct attributes
#define PACKED __attribute__( ( packed ) )

// Branch prediction hints
#define LIKELY( _expression ) __builtin_expect( !!( _expression ), 1 )
#define UNLIKELY( _expression ) __builtin_expect( !!( _expression ), 0 )

// Constants
#define COMMENT_SYMBOL ( '#' )
#define DECIMAL_RADIX 10
#define ONE_SECOND_IN_MILLISECONDS ( ( size_t )( 1000 ) )
#define ONE_MILLISECOND_IN_NANOSECONDS ( ( size_t )( 1000000 ) )
#define ASCII_COLOR_CYAN_LIGHT "\e[1;36m"
#define ASCII_COLOR_GREEN "\e[1;32m"
#define ASCII_COLOR_PURPLE_LIGHT "\e[1;35m"
#define ASCII_COLOR_RED "\e[1;31m"
#define ASCII_COLOR_YELLOW "\e[1;33m"
#define ASCII_COLOR_RESET_FOREGROUND "\e[39m"
#define ASCII_COLOR_RESET_BACKGROUND "\e[49m"
#define ASCII_COLOR_RESET "\e[0m"

// Utility macros ( no side-effects )
#define SECONDS_TO_MILLISECONDS( _seconds ) \
    ( ( _seconds ) * ONE_SECOND_IN_MILLISECONDS )
#define MILLISECONDS_TO_NANOSECONDS( _milliseconds ) \
    ( ( _milliseconds ) * ONE_MILLISECOND_IN_NANOSECONDS )
#define BITS_TO_BYTES( _bits ) ( ( size_t )( ( _bits ) / 8 ) )

// This macro turns a value into a string literal
#define STRINGIFY( _value ) #_value

// This is a helper macro that handles the stringify of macros
#define MACRO_TO_STRING( _macro ) STRINGIFY( _macro )

// Utility functions ( no side-effects )

// Utility functions ( side-effects )
#define ASSUME( _expression ) __builtin_assume( _expression )

#if ( defined( DEBUG ) && !defined( TESTS ) )

#if defined( assert )

#undef assert

#endif

#define TRAP_COLOR_LEVEL ASCII_COLOR_RED
#define TRAP_COLOR_FILE_NAME ASCII_COLOR_PURPLE_LIGHT
#define TRAP_COLOR_LINE_NUMBER ASCII_COLOR_PURPLE_LIGHT
#define TRAP_COLOR_FUNCTION_NAME ASCII_COLOR_PURPLE_LIGHT

#define BACKTRACE_LIMIT ( 5 )

template < typename... Arguments >
[[noreturn]] inline void _trap( std::format_string< Arguments... > _format,
                                const std::source_location _sourceLocation,
                                Arguments&&... _arguments ) {
    std::print(
        std::cerr,
        TRAP_COLOR_LEVEL
        "[TRAP] " ASCII_COLOR_RESET_FOREGROUND "Thread " TRAP_COLOR_THREAD_ID
        "{}" ASCII_COLOR_RESET_FOREGROUND ": File '" TRAP_COLOR_FILE_NAME
        "{}" ASCII_COLOR_RESET_FOREGROUND "': line " TRAP_COLOR_LINE_NUMBER
        "{}" ASCII_COLOR_RESET_FOREGROUND
        " in function '" TRAP_COLOR_FUNCTION_NAME
        "{}" ASCII_COLOR_RESET_FOREGROUND "' | Message: ",
        123, _sourceLocation.file_name(), _sourceLocation.line(),
        _sourceLocation.function_name() );
    std::println( std::cerr, _format,
                  std::forward< Arguments >( _arguments )... );
    // TODO: Print backtrace

    __builtin_trap();
}

// Function file:line | message
template < typename... Arguments >
inline void trap( std::format_string< Arguments... > _format,
                  Arguments&&... _arguments ) {
    _error( _format, std::source_location::current(),
            std::forward< Arguments >( _arguments )... );
}

#if 0
#define trap( ... )                                                      \
    do {                                                                 \
        write( STDERR_FILENO, DEBUG_INFORMATION1,                        \
               __builtin_strlen( DEBUG_INFORMATION1 ) );                 \
        write( STDERR_FILENO, __func__, __builtin_strlen( __func__ ) );  \
        write( STDERR_FILENO, DEBUG_INFORMATION2,                        \
               __builtin_strlen( DEBUG_INFORMATION2 ) );                 \
        __VA_OPT__( write( STDERR_FILENO, DEBUG_INFORMATION3,            \
                           __builtin_strlen( DEBUG_INFORMATION3 ) );     \
                    const char l_message[] = __VA_ARGS__;                \
                    write( STDERR_FILENO, l_message,                     \
                           __builtin_strlen( l_message ) ); );           \
        write( STDERR_FILENO, "\n", 1 );                                 \
        void* l_backtraceBuffer[ BACKTRACE_LIMIT ];                      \
        const size_t l_backtraceAmount =                                 \
            backtrace( l_backtraceBuffer, BACKTRACE_LIMIT );             \
        char** l_backtraceResolved =                                     \
            backtrace_symbols( l_backtraceBuffer, l_backtraceAmount );   \
        FOR_RANGE( size_t, 0, l_backtraceAmount ) {                      \
            char* l_backtrace = l_backtraceResolved[ _index ];           \
            char* l_fileNameEnd = __builtin_strchr( l_backtrace, '(' );  \
            *l_fileNameEnd = '\0';                                       \
            char* l_fileName = ( __builtin_strrchr( l_backtrace, '/' ) + \
                                 ( 1 * sizeof( char ) ) );               \
            write( STDERR_FILENO, l_fileName,                            \
                   __builtin_strlen( l_fileName ) );                     \
            *l_fileNameEnd = '(';                                        \
            write( STDERR_FILENO, l_fileNameEnd,                         \
                   __builtin_strlen( l_fileNameEnd ) );                  \
            write( STDERR_FILENO, ASCII_COLOR_RESET,                     \
                   __builtin_strlen( ASCII_COLOR_RESET ) );              \
            write( STDERR_FILENO, "\n", 1 );                             \
        }                                                                \
        free( l_backtraceResolved );                                     \
        __builtin_trap();                                                \
    } while ( 0 )
#endif

#define assert( _expression, ... ) \
    do {                           \
        if ( !( _expression ) ) {  \
            trap( __VA_ARGS__ );   \
        }                          \
    } while ( 0 )

#else

#define trap( ... ) ( ( void )0 )
#define assert( _expression, ... ) ( ( void )0 )

#endif

// Container utility functions
#define randomValueFromContainer( _array ) \
    ( ( _array )[ randomNumber() % std::size( _array ) ] )

// Utility functions ( no side-effects )

template < typename T >
inline constexpr auto lengthOfNumber( T _number ) -> bool {
    return ( ( ( ( _number ) < 10ULL ) )                     ? ( 1 )
             : ( ( ( _number ) < 100ULL ) )                  ? ( 2 )
             : ( ( ( _number ) < 1000ULL ) )                 ? ( 3 )
             : ( ( ( _number ) < 10000ULL ) )                ? ( 4 )
             : ( ( ( _number ) < 100000ULL ) )               ? ( 5 )
             : ( ( ( _number ) < 1000000ULL ) )              ? ( 6 )
             : ( ( ( _number ) < 10000000ULL ) )             ? ( 7 )
             : ( ( ( _number ) < 100000000ULL ) )            ? ( 8 )
             : ( ( ( _number ) < 1000000000ULL ) )           ? ( 9 )
             : ( ( ( _number ) < 10000000000ULL ) )          ? ( 10 )
             : ( ( ( _number ) < 100000000000ULL ) )         ? ( 11 )
             : ( ( ( _number ) < 1000000000000ULL ) )        ? ( 12 )
             : ( ( ( _number ) < 10000000000000ULL ) )       ? ( 13 )
             : ( ( ( _number ) < 100000000000000ULL ) )      ? ( 14 )
             : ( ( ( _number ) < 1000000000000000ULL ) )     ? ( 15 )
             : ( ( ( _number ) < 10000000000000000ULL ) )    ? ( 16 )
             : ( ( ( _number ) < 100000000000000000ULL ) )   ? ( 17 )
             : ( ( ( _number ) < 1000000000000000000ULL ) )  ? ( 18 )
             : ( ( ( _number ) < 10000000000000000000ULL ) ) ? ( 19 )
                                                             : ( 20 ) );
}

#define RANDOM_NUMBER_MAX SIZE_MAX

void randomNumber$seed$set( const size_t _seed );
auto randomNumber$seed$get() -> size_t;
auto randomNumber() -> size_t;

static inline auto generateHash( std::span< std::byte > _data ) -> size_t {
    size_t l_returnValue =
        XXH32( _data.data(), _data.size(), randomNumber$seed$get() );

    return ( l_returnValue );
}

template < typename T >
inline auto clone( T& _element ) {
    return ( std::make_unique< T >( _element ) );
}

void sanitizeString( std::string& _string );

// Utility OS specific functions ( no side-effects )
static inline auto getApplicationDirectoryAbsolutePath()
    -> std::optional< std::string > {
    std::optional< std::string > l_returnValue = std::nullopt;

    do {
        std::string l_executablePath;

        // Get executable path
        {
            const ssize_t l_executablePathLength = readlink(
                "/proc/self/exe", l_executablePath.data(), ( PATH_MAX - 1 ) );

            if ( UNLIKELY( l_executablePathLength == -1 ) ) {
                break;
            }
        }

        l_returnValue = std::filesystem::path( l_executablePath )
                            .remove_filename()
                            .string();
    } while ( false );

    return ( l_returnValue );
}

static inline constexpr auto getPathDirectory( std::string_view _path )
    -> std::string {
#if 0
    return ( std::string( dirname( std::string( _path ).data() ) ) );
#endif
    return ( std::filesystem::path( _path ).parent_path().string() );
}

static inline constexpr auto comparePathsDirectories( std::string_view _path1,
                                                      std::string_view _path2 )
    -> bool {
    return ( getPathDirectory( _path1 ) == getPathDirectory( _path2 ) );
}

auto getPathsByGlob( std::string_view _glob,
                     std::string_view _directory,
                     const bool _needSort ) -> std::vector< std::string >;

// Utility Compiler specific functions ( no side-effects )
// format - "%s%s %s = %p'
static inline void dumpCallback( void* _callback,
                                 void* _context,
                                 const char* _format,
                                 ... )
    __attribute__( ( format( printf,
                             3, // Format index
                             4  // First format argument index
                             ) ) ) {
    static size_t l_depth = 0;

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

            if ( UNLIKELY( !l_result ) ) {
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
auto iterateStructUnionTopMostFields( gsl::not_null< void* > _callback,
                                      gsl::not_null< void* > _context ) {
    T l_structSample;

    __builtin_dump_struct( &l_structSample, dumpCallback, _callback, _context );
}

} // namespace stdfunc
