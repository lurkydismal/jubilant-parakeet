#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <atomic>

#include "camera.hpp"
#include "input.hpp"
#include "stdfloat16.hpp"
#include "window.hpp"

namespace runtime {

using applicationState_t = struct applicationState {
    applicationState() = default;
    applicationState( const applicationState& ) = delete;
    applicationState( applicationState&& ) = delete;
    ~applicationState() = default;
    auto operator=( const applicationState& ) -> applicationState& = delete;
    auto operator=( applicationState&& ) -> applicationState& = delete;

    auto load() -> bool;
    auto unload() -> bool;

    struct {
        window::window_t window;
        SDL_Renderer* renderer = nullptr;
        camera::camera_t camera;

        size_t logicalWidth = 1280;
        size_t logicalHeight = 720;
        std::atomic< size_t > totalFramesRendered = 0;
    } renderContext;

    struct metadata {
        static constexpr float16_t g_version = 0.1;
        static constexpr std::string_view g_identifier =
            window::window_t::g_name;
    };

    // TODO: Write
    bool isPaused = false;
    bool status = false;
    input::input_t currentInput;
};

} // namespace runtime
