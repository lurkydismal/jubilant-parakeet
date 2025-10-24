#pragma once

#include <cstddef>
#include <string_view>

#include "log.hpp"
#include "slickdl.hpp"
#include "vsync.hpp"

namespace window {

using window_t = struct window {
    static constexpr std::string_view g_defaultName = "jubilant-parakeet";

    window( std::string_view _title = g_defaultName,
            slickdl::volume_t< int > _volume =
                {
                    640,
                    480,
                },
            SDL_WindowFlags _flags = SDL_WINDOW_INPUT_FOCUS )
        : name( _title ),
          width( _volume.width ),
          height( _volume.height ),
          handle( SDL_CreateWindow( std::string( _title ).c_str(),
                                    _volume.width,
                                    _volume.height,
                                    _flags ) ) {
        logg::info( "Window '{}' created", name );
    }

    // TODO: Implement
    constexpr window( slickdl::window_t _handle )
        : name( SDL_GetWindowTitle( _handle ) ),
          width( decltype( width ){} ),
          height( decltype( height ){} ),
          handle( _handle ) {
        SDL_GetWindowSizeInPixels( _handle, std::bit_cast< int* >( &width ),
                                   std::bit_cast< int* >( &height ) );

        logg::info( "Window '{}' created [from handle]", name );
    }

    window( const window& ) = delete;
    window( window&& ) = default;
    ~window() = default;
    auto operator=( const window& ) -> window& = delete;
    auto operator=( window&& ) -> window& = default;

    std::string_view name;
    size_t width;
    size_t height;
    size_t desiredFPS = 60;
    vsync::vsync_t vsync = vsync::vsync_t::software;
    size_t totalFramesRendered = 0;

    slickdl::window_t handle;
};

} // namespace window
