#pragma once

#include <array>
#include <bit>
#include <cstdint>

namespace color {

using color_t = struct color {
    color( uint8_t _red, uint8_t _green, uint8_t _blue, uint8_t _alpha )
        : red( _red ), green( _green ), blue( _blue ), alpha( _alpha ) {}

    color() = default;
    color( const color& ) = default;
    color( color&& ) = default;
    ~color() = default;

    color( uint32_t _colorPacked ) { unpack( _colorPacked ); }

    auto operator=( const color& ) -> color& = default;
    auto operator=( color&& ) -> color& = default;

    [[nodiscard]] constexpr auto pack() const -> uint32_t {
        std::array< uint8_t, 4 > l_bytes{};

        if constexpr ( std::endian::native == std::endian::little ) {
            l_bytes = { alpha, blue, green, red };

        } else {
            l_bytes = { red, green, blue, alpha };
        }

        return ( std::bit_cast< uint32_t >( l_bytes ) );
    }

    constexpr void unpack( uint32_t _colorPacked ) {
        const auto l_bytes =
            std::bit_cast< std::array< uint8_t, 4 > >( _colorPacked );

        if constexpr ( std::endian::native == std::endian::little ) {
            red = l_bytes[ 3 ];
            green = l_bytes[ 2 ];
            blue = l_bytes[ 1 ];
            alpha = l_bytes[ 0 ];

        } else {
            red = l_bytes[ 0 ];
            green = l_bytes[ 1 ];
            blue = l_bytes[ 2 ];
            alpha = l_bytes[ 3 ];
        }
    }

    uint8_t red{};
    uint8_t green{};
    uint8_t blue{};
    uint8_t alpha{};
};

} // namespace color
