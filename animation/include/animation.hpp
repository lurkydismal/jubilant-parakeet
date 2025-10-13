#pragma once

#include <SDL3/SDL_surface.h>

#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

#include "boxes.hpp"
#include "slickdl/render_texture.hpp"
#include "stddebug.hpp"

namespace animation {

using animation_t = struct animation {
    animation() = delete;
    animation( const animation& ) = default;
    animation( animation&& ) = default;
    ~animation() = default;

    constexpr animation( std::span< const slickdl::texture_t > _keyFrames,
                         std::span< const size_t > _frames,
                         boxes::boxes_t _targetBoxes )
        : _keyFrames( _keyFrames | std::ranges::to< std::vector >() ),
          _frames( _frames | std::ranges::to< std::vector >() ),
          _targetBoxes( std::move( _targetBoxes ) ) {
        stdfunc::assert( !this->_keyFrames.empty() );
        stdfunc::assert( !this->_frames.empty() );
    }

    constexpr animation( std::initializer_list< slickdl::texture_t > _keyFrames,
                         std::initializer_list< size_t > _frames,
                         const boxes::boxes_t& _targetBoxes )
        : animation( std::span( _keyFrames ),
                     std::span< const size_t >( _frames ),
                     _targetBoxes ) {}

    auto operator=( const animation& ) -> animation& = default;
    auto operator=( animation&& ) -> animation& = default;

    [[nodiscard]] constexpr auto currentKeyFrame() const -> slickdl::texture_t {
        stdfunc::assert( !_keyFrames.empty() );
        stdfunc::assert( !_frames.empty() );

        return ( _keyFrames.at( _frames.at( _currentFrame ) ) );
    }

    [[nodiscard]] constexpr auto currentTargetBox() const
        -> const boxes::box_t& {
        return ( _targetBoxes.currentFrame().front() );
    }

    constexpr void step( bool _canLoop ) {
        stdfunc::assert( !_frames.empty() );

        if ( _currentFrame < ( _frames.size() - 1 ) ) {
            _currentFrame++;

        } else {
            if ( _canLoop ) {
                _currentFrame = 0;
            }
        }

        _targetBoxes.step( _canLoop );
    }

    void render( const slickdl::renderer_t& _renderer,
                 const boxes::box_t& _targetBoxCoordinates ) const;

    void render( const slickdl::renderer_t& _renderer,
                 const boxes::box_t& _targetBoxCoordinates,
                 double _angle,
                 SDL_FlipMode _flipMode ) const;

    // Variables
private:
    std::vector< slickdl::texture_t > _keyFrames;

    // Indexes to key frames
    std::vector< size_t > _frames;
    size_t _currentFrame{};

    // Width and Height of key frames on screen
    boxes::boxes_t _targetBoxes;
};

} // namespace animation
