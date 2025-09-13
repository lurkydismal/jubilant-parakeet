#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <atomic>

#include "camera.hpp"
// #include "settings.hpp"

namespace runtime {

using applicationState_t = struct applicationState {
    applicationState() = default;
    applicationState( const applicationState& ) = default;
    applicationState( applicationState&& ) = default;
    ~applicationState() = default;
    auto operator=( const applicationState& ) -> applicationState& = default;
    auto operator=( applicationState&& ) -> applicationState& = default;

    auto load() -> bool { return ( true ); }
    auto unload() -> bool { return ( true ); }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
#if 0
    settings_t settings;
#endif
    camera::camera_t camera;
    size_t logicalWidth = 1280;
    size_t logicalHeight = 720;
    std::atomic< size_t > totalFramesRendered = 0;
    bool isPaused = false;
    bool status = false;
};

} // namespace runtime
