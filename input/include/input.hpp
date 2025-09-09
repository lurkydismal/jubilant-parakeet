#pragma once

#include <cstdint>
#include <string>

namespace input {

using direction_t = enum class direction : uint8_t {
    none = 0,
    up = 0b1,
    down = 0b10,
    left = 0b100,
    right = 0b1000,
};

[[nodiscard]] constexpr auto operator|( direction_t _storage,
                                        direction_t _value ) -> direction_t {
    using directionType_t = std::underlying_type_t< direction_t >;

    return ( static_cast< direction_t >(
        static_cast< directionType_t >( _storage ) |
        static_cast< directionType_t >( _value ) ) );
}

[[nodiscard]] constexpr auto operator&( direction_t _storage,
                                        direction_t _value ) -> direction_t {
    using directionType_t = std::underlying_type_t< direction_t >;

    return ( static_cast< direction_t >(
        static_cast< directionType_t >( _storage ) &
        static_cast< directionType_t >( _value ) ) );
}

constexpr void operator|=( direction_t& _storage, direction_t _value ) {
    _storage = ( _storage | _value );
}

// TODO: Implement
[[nodiscard]] inline auto toString( const direction_t& _direction )
    -> std::string {
    using directionType_t = std::underlying_type_t< direction_t >;

    return ( std::to_string( static_cast< directionType_t >( _direction ) ) );
}

using button_t = enum class button : uint8_t {
    none,
};

// TODO: Implement
[[nodiscard]] inline auto toString( const button_t& _button ) -> std::string {
    using buttonType_t = std::underlying_type_t< button_t >;

    return ( std::to_string( static_cast< buttonType_t >( _button ) ) );
}

using input_t = struct input {
    constexpr input( direction_t _direction, button_t _button = button_t::none )
        : direction( _direction ), button( _button ) {}
    constexpr input( button_t _button,
                     direction_t _direction = direction_t::none )
        : direction( _direction ), button( _button ) {}
    input() = default;
    input( const input& ) = default;
    input( input&& ) = default;
    ~input() = default;
    auto operator=( const input& ) -> input& = default;
    auto operator=( input&& ) -> input& = default;

    [[nodiscard]] constexpr auto operator==( const input& _input ) const
        -> bool {
        return ( ( direction == _input.direction ) &&
                 ( button == _input.button ) );
    }

    operator std::string() const {
        return ( toString( direction ) + toString( direction ) );
    }

    direction_t direction = direction_t::none;
    button_t button = button_t::none;
};

} // namespace input
