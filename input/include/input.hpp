#pragma once

#include <cstdint>
#include <format>
#include <string>

namespace input {

using direction_t = enum class direction : uint8_t {
    none = 0,
    up = 0b1,
    down = 0b10,
    left = 0b100,
    right = 0b1000,
};

using directionUnderlying_t = std::underlying_type_t< direction_t >;

[[nodiscard]] constexpr auto operator|( direction_t _storage,
                                        direction_t _value ) -> direction_t {
    return ( static_cast< direction_t >(
        static_cast< directionUnderlying_t >( _storage ) |
        static_cast< directionUnderlying_t >( _value ) ) );
}

[[nodiscard]] constexpr auto operator&( direction_t _storage,
                                        direction_t _value ) -> direction_t {
    return ( static_cast< direction_t >(
        static_cast< directionUnderlying_t >( _storage ) &
        static_cast< directionUnderlying_t >( _value ) ) );
}

using button_t = enum class button : uint8_t {
    none,
};

using buttonUnderlying_t = std::underlying_type_t< button_t >;

[[nodiscard]] constexpr auto operator|( button_t _storage, button_t _value )
    -> button_t {
    return ( static_cast< button_t >(
        static_cast< buttonUnderlying_t >( _storage ) |
        static_cast< buttonUnderlying_t >( _value ) ) );
}

[[nodiscard]] constexpr auto operator&( button_t _storage, button_t _value )
    -> button_t {
    return ( static_cast< button_t >(
        static_cast< buttonUnderlying_t >( _storage ) &
        static_cast< buttonUnderlying_t >( _value ) ) );
}

using input_t = struct input {
    input() = default;
    input( const input& ) = default;
    input( input&& ) = default;
    ~input() = default;

    constexpr input( direction_t _direction, button_t _button = button_t::none )
        : direction( _direction ), button( _button ) {}
    constexpr input( button_t _button,
                     direction_t _direction = direction_t::none )
        : direction( _direction ), button( _button ) {}

    auto operator=( const input& ) -> input& = default;
    auto operator=( input&& ) -> input& = default;

    [[nodiscard]] constexpr auto operator==( const input& _input ) const
        -> bool {
        return ( ( direction == _input.direction ) &&
                 ( button == _input.button ) );
    }

    direction_t direction = direction_t::none;
    button_t button = button_t::none;
};

} // namespace input

template <>
struct std::formatter< input::direction_t, char > {
    constexpr auto parse( std::format_parse_context& _context ) {
        return ( _context.begin() );
    }

    auto format( input::direction_t& _value,
                 std::format_context& _context ) const {
        return ( std::format_to( _context.out(), "{{}}", _value ) );
    }
};

template <>
struct std::formatter< input::button_t, char > {
    constexpr auto parse( std::format_parse_context& _context ) {
        return ( _context.begin() );
    }

    auto format( input::button_t& _value,
                 std::format_context& _context ) const {
        return ( std::format_to( _context.out(), "{{}}", _value ) );
    }
};

template <>
struct std::formatter< input::input_t, char > {
    constexpr auto parse( std::format_parse_context& _context ) {
        return ( _context.begin() );
    }

    auto format( input::input_t& _value, std::format_context& _context ) const {
        return ( std::format_to( _context.out(), "{}{}", _value.direction,
                                 _value.button ) );
    }
};
