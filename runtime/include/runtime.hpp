#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "camera.hpp"
#include "event.hpp"
#include "init.hpp"
#include "iterate.hpp"
#include "quit.hpp"
#include "settings_t.h"

namespace runtime {

using applicationState_t = struct applicationState {
    applicationState() = default;
    applicationState( const applicationState& ) = default;
    applicationState( applicationState&& ) = default;
    ~applicationState() = default;
    auto operator=( const applicationState& ) -> applicationState& = default;
    auto operator=( applicationState&& ) -> applicationState& = default;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    settings_t settings;
    camera::camera_t camera;
    size_t logicalWidth = 1280;
    size_t logicalHeight = 720;
    size_t totalFramesRendered = 0;
    bool isPaused = false;
    bool status = false;
};

} // namespace runtime
