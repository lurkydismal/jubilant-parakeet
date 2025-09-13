#pragma once

#include <SDL3/SDL_scancode.h>
#include <input.hpp>

namespace control {

using control_t = struct control {
    control( SDL_Scancode _scancode, input::input_t _input )
        : scancode( _scancode ), input( _input ) {}
    control( input::input_t _input ) : input( _input ) {}
    control() = default;
    control( const control& ) = default;
    control( control&& ) = default;
    ~control() = default;
    auto operator=( const control& ) -> control& = default;
    auto operator=( control&& ) -> control& = default;

    [[nodiscard]] constexpr auto operator==( const control& _control ) const
        -> bool {
        return ( ( scancode == _control.scancode ) &&
                 ( input == _control.input ) );
    }

    // TODO: Implement
    operator ::std::string() const {
        return ( std::to_string( scancode ) + std::string( input ) );
    }

    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
    input::input_t input;
};

} // namespace control
