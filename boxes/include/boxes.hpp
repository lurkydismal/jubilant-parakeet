#pragma once

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <algorithm>
#include <gsl/pointers>
#include <ranges>

#include "color.hpp"
#include "log.hpp"
#include "stdfunc.hpp"

namespace boxes {

using box_t = struct box {
    box( float _x, float _y, float _width, float _height )
        : x( _x ), y( _y ), width( _width ), height( _height ) {
#if !defined( TESTS )

        logg::debug(
            "Box properties: X = {}, Y = {}, Width = {}"
            ", Heigth = {}",
            _x, _y, _width, _height );

#endif
    }

    box() = delete;
    box( const box& ) = default;
    box( box&& ) = default;
    ~box() = default;
    auto operator=( const box& ) -> box& = default;
    auto operator=( box&& ) -> box& = default;

    float x;
    float y;
    float width;
    float height;
};

using boxes_t = struct boxes {
    boxes() = delete;
    boxes( const boxes& ) = default;
    boxes( boxes&& ) = default;
    ~boxes() = default;

    boxes( std::span< std::span< const box_t > > _boxesKeyFrames ) {
        stdfunc::assert( !_boxesKeyFrames.empty() );
        stdfunc::assert( std::ranges::none_of(
            _boxesKeyFrames,
            []( std::span< const box_t > _boxKeyFrames ) -> bool {
                return ( _boxKeyFrames.empty() );
            } ) );

        _frames = _boxesKeyFrames |
                  std::ranges::to< std::vector< std::vector< box_t > > >();
    }

    auto operator=( const boxes& ) -> boxes& = default;
    auto operator=( boxes&& ) -> boxes& = default;

    [[nodiscard]] constexpr auto color() -> auto& { return ( _color ); }

    [[nodiscard]] constexpr auto currentKeyFrame() -> std::span< const box_t > {
        return ( _frames.at( _currentFrame ) );
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
    }

    constexpr void render( gsl::not_null< SDL_Renderer* > _renderer,
                           const box_t& _screenSpaceTarget,
                           bool _doFill ) {
        color::color_t l_colorBefore;

        // Store current draw color
        {
            uint8_t l_red = 0;
            uint8_t l_green = 0;
            uint8_t l_blue = 0;
            uint8_t l_alpha = 0;

            const bool l_result = SDL_GetRenderDrawColor(
                _renderer, &l_red, &l_green, &l_blue, &l_alpha );

            stdfunc::assert( l_result, "Getting renderer draw color: '{}'",
                             SDL_GetError() );

            l_colorBefore = color::color_t( l_red, l_green, l_blue, l_alpha );
        }

        // Set current draw color
        {
            const bool l_result =
                SDL_SetRenderDrawColor( _renderer, _color.red, _color.green,
                                        _color.blue, _color.alpha );

            stdfunc::assert( l_result, "Setting renderer draw color: '{}'",
                             SDL_GetError() );
        }

        const auto l_currentFrame = currentKeyFrame();

        for ( const auto& _box : l_currentFrame ) {
            const SDL_FRect l_targetRectangle = {
                .x = ( _screenSpaceTarget.x + _box.x ),
                .y = ( _screenSpaceTarget.y + _box.y ),
                .w = _box.width,
                .h = _box.height,
            };

            if ( _doFill ) {
                const bool l_result =
                    SDL_RenderFillRect( _renderer, &l_targetRectangle );

                stdfunc::assert( l_result, "Render filled rectangle: '{}'",
                                 SDL_GetError() );

            } else {
                const bool l_result =
                    SDL_RenderRect( _renderer, &l_targetRectangle );

                stdfunc::assert( l_result, "Render rectangle: '{}'",
                                 SDL_GetError() );
            }
        }

        // Load current draw color
        {
            const bool l_result = SDL_SetRenderDrawColor(
                _renderer, l_colorBefore.red, l_colorBefore.green,
                l_colorBefore.blue, l_colorBefore.alpha );

            stdfunc::assert( l_result );
        }
    }

private:
    // Frame is a list of boxes
    using frame_t = std::vector< box_t >;

    std::vector< frame_t > _frames;
    size_t _currentFrame{};
    color::color_t _color{};
};

} // namespace boxes
