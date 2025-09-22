#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

#include <gsl/pointers>
#include <utility>

#include "boxes.hpp"
#include "stdfunc.hpp"

namespace animation {

// FIX: Move to slickdl
using texture_t = gsl::not_null< SDL_Texture* >;

using animation_t = struct animation {
    animation() = delete;
    animation( const animation& ) = default;
    animation( animation&& ) = default;
    ~animation() = default;
    auto operator=( const animation& ) -> animation& = default;
    auto operator=( animation&& ) -> animation& = default;

    animation( std::span< const texture_t > _keyFrames,
               std::span< size_t > _frames,
               boxes::boxes_t _targetBoxes )
        : _keyFrames( _keyFrames |
                      std::ranges::to< std::vector< texture_t > >() ),
          _targetBoxes( std::move( _targetBoxes ) ) {
        stdfunc::assert( !_keyFrames.empty() );
        stdfunc::assert( !_frames.empty() );
    }

    [[nodiscard]] constexpr auto currentKeyFrame() const -> texture_t {
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

    void render( gsl::not_null< SDL_Renderer* > _renderer,
                 const boxes::box_t& _targetBoxCoordinates ) const {
        _render( _renderer, _targetBoxCoordinates, SDL_RenderTexture );
    }

    void render( gsl::not_null< SDL_Renderer* > _renderer,
                 const boxes::box_t& _targetBoxCoordinates,
                 double _angle,
                 SDL_FlipMode _flipMode ) const {
        _render( _renderer, _targetBoxCoordinates, SDL_RenderTextureRotated,
                 _angle, nullptr, _flipMode );
    }

    // Helpers
private:
    template < typename... Arguments >
    void _render( gsl::not_null< SDL_Renderer* > _renderer,
                  const boxes::box_t& _targetBoxCoordinates,
                  auto _renderFunction,
                  Arguments&&... _arguments ) const {
        const boxes::box_t& l_targetBoxSizes = currentTargetBox();

        const SDL_FRect l_resolvedTargetRectangle = {
            _targetBoxCoordinates.x, _targetBoxCoordinates.y,
            l_targetBoxSizes.width, l_targetBoxSizes.height };

        const texture_t l_keyFrame = currentKeyFrame();

        // Render
        {
            const bool l_result = _renderFunction(
                _renderer, l_keyFrame, nullptr, &l_resolvedTargetRectangle,
                std::forward< Arguments >( _arguments )... );

            stdfunc::assert( l_result, "Rendering texture: '{}'",
                             SDL_GetError() );
        }
    }

    // Variables
private:
    std::vector< texture_t > _keyFrames;

    // Indexes to key frames
    std::vector< size_t > _frames;
    size_t _currentFrame{};

    // Width and Height of key frames on screen
    boxes::boxes_t _targetBoxes;
};

} // namespace animation
