#pragma once

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>

#include <string>
#include <string_view>

#include "input.hpp"

namespace control {

using control_t = struct control {
    constexpr control( SDL_Scancode _scancode, input::input_t _input )
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
    operator std::string() const {
        std::string_view l_keyName = SDL_GetKeyName( scancode );

        if ( l_keyName.empty() ) {
            l_keyName = "UNKNOWN";
        }

#if 0
        return ( std::format( "{} - {}", l_keyName, input ) );
#endif
        return ( std::string( l_keyName ) );
    }

    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
    input::input_t input;
};

} // namespace control
