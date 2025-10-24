#pragma once

#include <SDL3/SDL_surface.h>

#include <utility>

#include "animation.hpp"
#include "boxes.hpp"
#include "slickdl/line_box.hpp"
#include "slickdl/render_texture.hpp"

namespace state {

using state_t = struct state {
    state() = delete;
    state( const state& ) = delete;

    state( state&& ) = default;
    ~state() = default;

    constexpr state( animation::animation_t&& _animation,
                     boxes::boxes_t&& _boxes,
                     bool _isActionable,
                     bool _canLoop,
                     slickdl::renderer_t& _renderer )
        : _animation( std::move( _animation ) ),
          _boxes( std::move( _boxes ) ),
          _isActionable( _isActionable ),
          _canLoop( _canLoop ),
          _renderer( _renderer ) {}

    auto operator=( const state& ) -> state& = delete;
    auto operator=( state&& ) -> state& = default;

    [[nodiscard]] constexpr auto isActionable() const -> bool {
        return ( _isActionable );
    }

    constexpr void step() {
        _animation.step( _canLoop );
        _boxes.step( _canLoop );
    }

    void render( const slickdl::box_t< float >& _cameraBoxCoordinates,
                 bool _doDrawBoxes,
                 bool _doFillBoxes );

    void render( const slickdl::box_t< float >& _cameraBoxCoordinates,
                 bool _doDrawBoxes,
                 bool _doFillBoxes,
                 double _angle,
                 SDL_FlipMode _flipMode );

    // Variables
private:
    animation::animation_t _animation;
    boxes::boxes_t _boxes;
    bool _isActionable;
    bool _canLoop;
    slickdl::renderer_t _renderer;
};

} // namespace state
