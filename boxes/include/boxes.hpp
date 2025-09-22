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

    boxes( std::span< std::span< const box_t > > _frames )
        : _frames( _frames | std::ranges::to< std::vector< frame_t > >() ) {
        stdfunc::assert( !_frames.empty() );
        stdfunc::assert( std::ranges::none_of(
            _frames, []( std::span< const box_t > _frame ) -> bool {
                return ( _frame.empty() );
            } ) );
    }

    auto operator=( const boxes& ) -> boxes& = default;
    auto operator=( boxes&& ) -> boxes& = default;

    [[nodiscard]] constexpr auto color() const -> auto& { return ( _color ); }

    [[nodiscard]] constexpr auto currentFrame() const
        -> std::span< const box_t > {
        stdfunc::assert( !_frames.empty() );

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

    void render( gsl::not_null< SDL_Renderer* > _renderer,
                 const box_t& _screenSpaceTarget,
                 bool _doFill ) const {
        color::color_t l_colorBefore;

        // Store current draw color
        {
            const bool l_result = SDL_GetRenderDrawColor(
                _renderer, &l_colorBefore.red, &l_colorBefore.green,
                &l_colorBefore.blue, &l_colorBefore.alpha );

            stdfunc::assert( l_result, "Getting renderer draw color: '{}'",
                             SDL_GetError() );
        }

        const auto l_setRenderDrawColor =
            [ & ]( auto _function, const color::color_t& _color ) -> void {
            const bool l_result =
                _function( _renderer, _color.red, _color.green, _color.blue,
                           _color.alpha );

            stdfunc::assert( l_result, "Setting renderer draw color: '{}'",
                             SDL_GetError() );
        };

        l_setRenderDrawColor( SDL_SetRenderDrawColor, _color );

        const auto l_currentFrame = currentFrame();

        for ( const auto& _box : l_currentFrame ) {
            const SDL_FRect l_targetRectangle = {
                .x = ( _screenSpaceTarget.x + _box.x ),
                .y = ( _screenSpaceTarget.y + _box.y ),
                .w = _box.width,
                .h = _box.height,
            };

            const auto l_render = [ & ]( auto _renderFunction ) -> void {
                const bool l_result =
                    _renderFunction( _renderer, &l_targetRectangle );

                stdfunc::assert( l_result, "Render rectangle: '{}'",
                                 SDL_GetError() );
            };

            l_render( ( _doFill ) ? ( SDL_RenderFillRect )
                                  : ( SDL_RenderRect ) );
        }

        l_setRenderDrawColor( SDL_SetRenderDrawColor, l_colorBefore );
    }

private:
    // Frame is a list of boxes
    using frame_t = std::vector< box_t >;

    std::vector< frame_t > _frames;
    size_t _currentFrame{};
    color::color_t _color{};
};

} // namespace boxes
