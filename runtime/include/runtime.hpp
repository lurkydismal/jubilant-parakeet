#pragma once

#include <cstddef>
#include <string_view>
#include <utility>

#include "camera.hpp"
#include "input.hpp"
#include "slickdl/render_texture.hpp"
#include "window.hpp"

namespace runtime {

using applicationState_t = struct applicationState {
    struct r {
        r( window::window_t&& _window, slickdl::renderer_t&& _renderer )
            : window( std::move( _window ) ),
              renderer( std::move( _renderer ) ) {}

        constexpr r( slickdl::window_t _window,
                     slickdl::renderer_t&& _renderer )
            : window( _window ), renderer( std::move( _renderer ) ) {}

        window::window_t window;
        slickdl::renderer_t renderer;
        camera::camera_t camera;

        size_t logicalWidth = 1280;
        size_t logicalHeight = 720;
    } renderContext;

    struct metadata {
        static constexpr std::string_view g_identifier =
            window::window_t::g_defaultName;
        static constexpr std::string_view g_description = "TODO: Write";
        static constexpr float g_version = 0.1;
        static constexpr std::string_view g_contactAddress =
            "lurkydismal@duck.com";
    };

    applicationState( window::window_t&& _window,
                      slickdl::renderer_t&& _renderer )
        : renderContext( std::move( _window ), std::move( _renderer ) ) {}

    constexpr applicationState( slickdl::window_t _window,
                                slickdl::renderer_t&& _renderer )
        : renderContext( _window, std::move( _renderer ) ) {}

    applicationState( const applicationState& ) = delete;
    applicationState( applicationState&& ) = default;

    ~applicationState() = default;

    auto operator=( const applicationState& ) -> applicationState& = delete;
    auto operator=( applicationState&& ) -> applicationState& = default;

    auto load() -> bool;
    auto unload() -> bool;

    // TODO: Write
    bool isPaused = false;
    bool status = false;
    input::input_t currentInput;
};

} // namespace runtime
