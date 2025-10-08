#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <span>
#include <vector>

#include "color.hpp"
#include "slickdl.hpp"
#include "stddebug.hpp"

#if !defined( TESTS )

#include "log.hpp"

#endif

namespace boxes {

using box_t = struct box {
    constexpr box( float _x, float _y, float _width, float _height )
        : x( _x ), y( _y ), width( _width ), height( _height ) {
        stdfunc::assert( _x );
        stdfunc::assert( _y );
        stdfunc::assert( _width );
        stdfunc::assert( _height );

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

    constexpr boxes( std::span< std::span< const box_t > > _frames )
        : _frames( _frames | std::ranges::to< std::vector< frame_t > >() ) {
        stdfunc::assert( !_frames.empty() );
        stdfunc::assert( std::ranges::none_of(
            _frames, []( std::span< const box_t > _frame ) -> bool {
                return ( _frame.empty() );
            } ) );
    }

    // TODO: Improve
    constexpr boxes(
        std::initializer_list< std::initializer_list< box_t > > _frames )
        : _frames( _frames | std::ranges::to< std::vector< frame_t > >() ) {
        stdfunc::assert( !this->_frames.empty() );
        stdfunc::assert( std::ranges::none_of(
            this->_frames, []( const frame_t& _frame ) -> bool {
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

    void render( const slickdl::renderer_t& _renderer,
                 const box_t& _screenSpaceTarget,
                 bool _doFill ) const;

private:
    // Frame is a list of boxes
    using frame_t = std::vector< box_t >;

    std::vector< frame_t > _frames;
    size_t _currentFrame{};
    color::color_t _color{};
};

} // namespace boxes
