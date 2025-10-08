#pragma once

#include <SDL3/SDL_video.h>
#include <stddef.h>

#include <string_view>

#include "vsync.hpp"

namespace window {

using window_t = struct window {
    window() = default;
    window( const window& ) = default;
    window( window&& ) = default;
    ~window() = default;
    auto operator=( const window& ) -> window& = default;
    auto operator=( window&& ) -> window& = default;

    static constexpr std::string_view g_name = "jubilant-parakeet";
    size_t width = 640;
    size_t height = 480;
    size_t desiredFPS = 60;
    vsync::vsync_t vsync = vsync::vsync_t::software;

    SDL_Window* handle = nullptr;
};

} // namespace window
