#pragma once

#include <glaze/glaze.hpp>
#include <xxhash.h>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <gsl/pointers>
#include <random>
#include <ranges>
#include <regex>
#include <string_view>
#include <type_traits>
#include <utility>

#include "ctll.hpp"
#include "ctre.hpp"

#if defined( DEBUG )

#include <iostream>
#include <print>
// #include <stacktrace>
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
constexpr char g_commentSymbol = '#';
constexpr size_t g_decimalRadix = 10;

namespace color {

constexpr std::string_view g_cyanLight = "\x1b[1;36m";
constexpr std::string_view g_blueLight = "\x1b[1;34m";
constexpr std::string_view g_green = "\x1b[1;32m";
constexpr std::string_view g_purpleLight = "\x1b[1;35m";
constexpr std::string_view g_red = "\x1b[1;31m";
constexpr std::string_view g_yellow = "\x1b[1;33m";
constexpr std::string_view g_resetForeground = "\x1b[39m";
constexpr std::string_view g_resetBackground = "\x1b[49m";
constexpr std::string_view g_reset = "\x1b[0m";

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

#if defined( DEBUG )

namespace {

constexpr std::string_view g_trapColorLevel = color::g_red;
constexpr std::string_view g_trapColorThreadId = color::g_purpleLight;
constexpr std::string_view g_trapColorFileName = color::g_purpleLight;
constexpr std::string_view g_trapColorLineNumber = color::g_purpleLight;
constexpr std::string_view g_trapColorFunctionName = color::g_purpleLight;

constexpr size_t g_backtraceLimit = 10;

constexpr auto formatWithColor( auto _what, std::string_view _color )
    -> std::string {
    return ( std::format( "{}{}{}", _color, _what,
                          stdfunc::color::g_resetForeground ) );
}

} // namespace

template < typename... Arguments >
[[noreturn]] void trap( std::format_string< Arguments... > _format = "",
                        Arguments&&... _arguments ) {
    std::print( std::cerr, "{} Thread {}{:#X}{}:",
                formatWithColor( "[TRAP]", g_trapColorLevel ),
                g_trapColorThreadId,
                std::hash< std::thread::id >{}( std::this_thread::get_id() ),
                color::g_reset );
    std::println( std::cerr, _format,
                  std::forward< Arguments >( _arguments )... );
#if 0
    // FIX: Undefined symbol in debug build
    std::println( "{}", std::stacktrace::current( 2, g_backtraceLimit ) );
#endif

    __builtin_trap();
}

template < typename... Arguments >
constexpr void assert( bool _result,
                       std::format_string< Arguments... > _format = "",
                       Arguments&&... _arguments ) {
    if ( !_result ) [[unlikely]] {
        trap( _format, std::forward< Arguments >( _arguments )... );
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
             std::views::take_while( []( char _symbol ) constexpr -> bool {
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
constexpr auto number( T _min, T _max ) -> T {
    using distribution_t =
        std::conditional_t< std::is_integral_v< T >,
                            std::uniform_int_distribution< T >,
                            std::uniform_real_distribution< T > >;

    return ( ( distribution_t( _min, _max ) )( g_engine ) );
}

template < typename T >
    requires std::is_arithmetic_v< T >
constexpr auto number() -> T {
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
constexpr auto value( Container& _container ) ->
    typename Container::value_type& {
    assert( !_container.empty() );

    return ( _container.at( number< size_t >( 0, _container.size() - 1 ) ) );
}

template < typename Container >
    requires is_container< Container >
constexpr auto value( const Container& _container ) -> const
    typename Container::value_type& {
    assert( !_container.empty() );

    return ( _container.at( number< size_t >( 0, _container.size() - 1 ) ) );
}

template < typename Container >
    requires is_container< Container >
constexpr auto view( Container& _container ) {
    assert( !_container.empty() );

    return ( std::views::iota( 0 ) |
             std::views::transform( [ & ]( auto ) constexpr -> auto {
                 return ( value( _container ) );
             } ) );
}

template < typename Container >
    requires is_container< Container >
constexpr auto view( const Container& _container ) {
    assert( !_container.empty() );

    return ( std::views::iota( 0 ) |
             std::views::transform( [ & ]( auto ) constexpr -> auto {
                 return ( value( _container ) );
             } ) );
}

template < typename Container, typename T = typename Container::value_type >
    requires is_container< Container > && std::is_arithmetic_v< T >
constexpr void fill( Container& _container, T _min, T _max ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( number< T >( _min, _max ) );
    } );
}

template < typename Container, typename T = typename Container::value_type >
    requires is_container< Container > && std::is_same_v< T, std::byte >
constexpr void fill( Container& _container, uint8_t _min, uint8_t _max ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( static_cast< std::byte >( number< uint8_t >( _min, _max ) ) );
    } );
}

template < typename Container, typename T = typename Container::value_type >
    requires is_container< Container > && std::is_arithmetic_v< T >
constexpr void fill( Container& _container ) {
    std::ranges::generate(
        _container, [ & ] constexpr -> auto { return ( number< T >() ); } );
}

template < typename Container, typename T = typename Container::value_type >
    requires is_container< Container > && std::is_same_v< T, std::byte >
constexpr void fill( Container& _container ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( static_cast< std::byte >( number< uint8_t >() ) );
    } );
}

} // namespace random

[[nodiscard]] constexpr auto generateHash( std::span< std::byte > _data,
                                           size_t _seed = random::g_engine() )
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

    return ( Container< variant_t, sizeof...( Arguments ) >{
        variant_t( std::forward< Arguments >( _arguments ) )... } );
}

namespace filesystem {

// Utility OS specific functions ( no side-effects )
[[nodiscard]] auto getApplicationDirectoryAbsolutePath()
    -> std::optional< std::filesystem::path >;

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

    return ( _getPathsByRegexp(
        _directory, [ & ]( const std::string& _fileName ) -> bool {
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

    return ( _getPathsByRegexp( _directory,
                                [ & ]( const std::string& _fileName ) -> auto {
                                    return ( l_matcher( _fileName ) );
                                } ) );
}

} // namespace filesystem

namespace compress {

/**
 * @brief Compress a UTF-8 (or arbitrary) text string.
 *
 * This is a convenience wrapper around a fast, stream-oriented text compressor
 * (current implementation: **Snappy**). The function returns a `std::string`
 * containing the compressed frame. The compressed output is binary data and
 * may contain NUL bytes — treat it as opaque.
 *
 * @param _text  Input text to compress. Treated as a sequence of bytes; no
 *               string encoding validation is performed.
 * @param _level Compression level: Level 1 is the fastest Level 2 is a little
 *               slower but provides better compression.
 *
 * @return `std::optional<std::string>`:
 *         - contains compressed bytes on success;
 *         - `std::nullopt` on failure (e.g. underlying library error,
 *           out-of-memory, or other API-level failure).
 *
 * @threadsafe Safe to call concurrently from multiple threads as long as the
 *            underlying compressor library is initialized appropriately and
 *            you do not mutate any shared global compressor state. The
 *            implementation should allocate thread-local temporaries.
 *
 * @complexity Time: roughly linear in `_text.size()`; exact constant factors
 *             depend on compressor. Memory: allocates an output buffer sized
 *             to the compressor's worst-case output (implementation dependent).
 *
 * @example
 * std::optional<std::string> c = compress::text("hello world");
 * if (c) {
 *     // c->data() contains compressed bytes (not a printable string)
 * }
 */
// TODO: Make constexpr
[[nodiscard]] auto text( std::string_view _text, size_t _level = 1 )
    -> std::optional< std::string >;

/**
 * @brief Compress arbitrary binary data.
 *
 * This function compresses a block of bytes using a general-purpose binary
 * compressor (current implementation: **Zstandard (zstd)**). It returns a
 * vector of bytes containing the compressed frame. The result should be
 * treated as opaque binary data.
 *
 * @param _data  Input data to compress (view into caller-owned memory).
 * @param _level Compression level. For zstd typical valid values are
 *               1..22 (implementation-dependent). Lower values favor speed,
 *               higher values favor compression ratio. Default is `3`
 *               (a reasonable speed/ratio tradeoff).
 *
 *               Rough guideline (zstd):
 *                 - 1 : fastest, lowest compression
 *                 - 3 : fast, good default
 *                 - 9 : slower, noticeably better compression
 *                 - 19-22 : slowest, best compression (may be very slow/memory
 * heavy)
 *
 * @return `std::optional<std::vector<std::byte>>`:
 *         - contains compressed bytes on success;
 *         - `std::nullopt` on failure (invalid arguments, compression error,
 *           or underlying library failure).
 *
 * @threadsafe Thread-safe to call concurrently unless the implementation uses
 *            a shared mutable compressor context (it typically won't). If you
 *            plan high-concurrency workloads, benchmark and consider per-thread
 *            contexts or streaming APIs.
 *
 * @complexity Time: roughly linear in `_data.size()` with constants depending
 *             on `_level`. Memory: allocates output buffer of size proportional
 *             to compressor worst-case.
 *
 * @example
 * std::vector<std::byte> in = ...;
 * auto outOpt = compress::data(std::span(in), level=5);
 * if (outOpt) { write outOpt->data() to disk/network  }
 */
// TODO: Make constexpr
[[nodiscard]] auto data( std::span< std::byte > _data, size_t _level = 3 )
    -> std::optional< std::vector< std::byte > >;

} // namespace compress

namespace decompress {

/**
 * @brief Decompress a text frame produced by `compress::text`.
 *
 * Attempts to decompress the binary frame in `_data` and return the original
 * text as `std::string`. Returns `std::nullopt` on failure (invalid frame,
 * corruption, unsupported format, or other decompression error).
 *
 * @param _data Compressed frame (binary). The function treats the bytes as
 *              opaque input for the decompressor.
 *
 * @return `std::optional<std::string>`:
 *         - decompressed original string on success;
 *         - `std::nullopt` on failure.
 *
 * @threadsafe Safe to call concurrently from multiple threads provided the
 *            underlying library does not require exclusive initialization.
 *
 * @complexity Time: roughly linear in the size of the decompressed data.
 *
 * @note If `compress::text` is implemented with Snappy (no framing for original
 *       size), the decompressor will use the framing/format produced by that
 *       compressor. If your compressed frames do not include the original
 *       size, the decompressor must rely on the compressor frame metadata.
 *
 * @example
 * auto de = decompress::text(compressed_string);
 * if (de) { std::string s = std::move(*de); }
 */
// TODO: Make constexpr
[[nodiscard]] auto text( std::string_view _data )
    -> std::optional< std::string >;

/**
 * @brief Decompress binary data produced by `compress::data`.
 *
 * Decompress a binary compressed frame into a `std::vector<std::byte>` of the
 * original size `_originalSize`. Some compressors (like zstd) may store the
 * original size in the frame; however many APIs expect the caller to provide
 * the expected decompressed size. If your compressor stores the original size
 * in the frame, `_originalSize` can be unused — consult your implementation.
 *
 * @param _data         Compressed frame bytes.
 * @param _originalSize Expected size of the decompressed output in bytes.
 *                      The function may use this to allocate the output buffer
 *                      and to validate the decompressed result. Passing an
 *                      incorrect `_originalSize` may cause decompression to
 *                      fail or return truncated/incorrect data.
 *
 * @return `std::optional<std::vector<std::byte>>`:
 *         - decompressed byte vector on success;
 *         - `std::nullopt` on failure (bad frame, corruption, mismatch with
 *           `_originalSize`, etc.).
 *
 * @threadsafe Safe to call concurrently, subject to underlying library rules.
 *
 * @complexity Time: approximately linear in `_originalSize` (or the
 *             decompressed amount).
 *
 * @example
 * auto outOpt = decompress::data(std::span(compressedBytes),
 * expected_original_size); if (outOpt) { write(outOpt->data(), outOpt->size());
 * }
 */
// TODO: Make constexpr
[[nodiscard]] auto data( std::span< std::byte > _data, size_t _originalSize )
    -> std::optional< std::vector< std::byte > >;

} // namespace decompress

namespace meta {

template < typename T >
concept is_reflectable = ( glz::reflectable< T > || glz::glaze_object_t< T > );

/**
 * @brief Convert type into struct with metadata
 *
 * If T is struct or class, then fields are
 * struct reflect< T > {
 *       static constexpr auto size = 0;
 *       static constexpr auto values = tuple{};
 *       static constexpr std::array<sv, 0> keys{};
 *
 *       template <size_t I>
 *       using type = std::nullptr_t;
 * }
 */
template < typename T >
using reflect_t = glz::reflect< T >;

// If you want to make an empty struct or a struct with constructors work with
// reflection, add the folllwing constructor to your type:
// myStruct( stdfunc::make_reflectable ) {}
using makeReflectable_t = glz::make_reflectable;

// Functions that take instance
template < typename T, typename Callback >
    requires is_reflectable< T >
constexpr void iterateStructTopMostFields( T&& _instance,
                                           Callback&& _callback ) {
    glz::for_each_field( std::forward< T >( _instance ),
                         std::forward< Callback >( _callback ) );
}

// Functions that take type
template < typename T, typename Callback >
    requires is_reflectable< T >
constexpr void iterateStructTopMostFields( Callback&& _callback ) {
    T l_instance{};

    glz::for_each_field( l_instance, std::forward< Callback >( _callback ) );
}

template < typename T >
    requires is_reflectable< T >
consteval auto hasMemberWithName( std::string_view _name ) -> bool {
    return ( std::ranges::contains( reflect_t< T >::keys, _name ) );
}

} // namespace meta

} // namespace stdfunc
