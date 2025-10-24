#pragma once

#include <string>
#include <string_view>

#include "input.hpp"
#include "slickdl/keyboard.hpp"
#include "slickdl/scancode.hpp"

namespace control {

using control_t = struct control {
    control() = default;
    control( const control& ) = default;
    control( control&& ) = default;
    ~control() = default;

    constexpr control( input::input_t _input ) : input( _input ) {}
    constexpr control( slickdl::scancode_t _scancode, input::input_t _input )
        : scancode( _scancode ), input( _input ) {}

    auto operator=( const control& ) -> control& = default;
    auto operator=( control&& ) -> control& = default;

    [[nodiscard]] constexpr auto operator==( const control& _control ) const
        -> bool {
        return ( ( scancode == _control.scancode ) &&
                 ( input == _control.input ) );
    }

    // TODO: Implement
    operator std::string() const {
        std::string_view l_keyName = slickdl::keyboard::keyName(
            slickdl::keyboard::scancodeToKeycode( scancode ) );

#if 0
        return ( std::format( "{} - {}", l_keyName, input ) );
#endif
        return ( std::string( l_keyName ) );
    }

    slickdl::scancode_t scancode = slickdl::scancode_t::unknown;
    input::input_t input;
};

} // namespace control
