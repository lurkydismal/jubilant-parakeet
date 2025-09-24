#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_video.h>

#include <gsl/pointers>
#include <utility>

#include "animation.hpp"
#include "boxes.hpp"

namespace state {

using state_t = struct state {
    state() = delete;
    state( const state& ) = default;
    state( state&& ) = default;
    ~state() = default;

    constexpr state( animation::animation_t _animation,
                     boxes::boxes_t _boxes,
                     bool _isActionable,
                     gsl::not_null< SDL_Renderer* > _renderer )
        : _animation( std::move( _animation ) ),
          _boxes( std::move( _boxes ) ),
          _isActionable( _isActionable ),
          _renderer( _renderer ) {}

    auto operator=( const state& ) -> state& = default;
    auto operator=( state&& ) -> state& = default;

    [[nodiscard]] constexpr auto isActionable() const -> bool {
        return ( _isActionable );
    }

    constexpr void step() {
        _animation.step( _canLoop );
        _boxes.step( _canLoop );
    }

    void render( const boxes::box_t& _cameraBoxCoordinates,
                 bool _doDrawBoxes,
                 bool _doFillBoxes ) const {
        _render( _cameraBoxCoordinates, _doDrawBoxes, _doFillBoxes );
    }

    void render( const boxes::box_t& _cameraBoxCoordinates,
                 bool _doDrawBoxes,
                 bool _doFillBoxes,
                 double _angle,
                 SDL_FlipMode _flipMode ) const {
        _render( _cameraBoxCoordinates, _doDrawBoxes, _doFillBoxes, _angle,
                 _flipMode );
    }

    // Helpers
private:
    template < typename... Arguments >
    void _render( const boxes::box_t& _cameraBoxCoordinates,
                  bool _doDrawBoxes,
                  bool _doFillBoxes,
                  Arguments&&... _arguments ) const {
        boxes::box_t l_targetBox = _animation.currentTargetBox();

        l_targetBox.x += _cameraBoxCoordinates.x;
        l_targetBox.y += _cameraBoxCoordinates.y;

        _animation.render( _renderer, l_targetBox,
                           std::forward< Arguments >( _arguments )... );

        if ( _doDrawBoxes ) {
            _boxes.render( _renderer, l_targetBox, _doFillBoxes );
        }
    }

    // Variables
private:
    animation::animation_t _animation;
    boxes::boxes_t _boxes;
    bool _isActionable;
    bool _canLoop{};
    gsl::not_null< SDL_Renderer* > _renderer;
};

} // namespace state
