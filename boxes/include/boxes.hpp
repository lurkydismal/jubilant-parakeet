#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <span>
#include <vector>

#include "slickdl/color.hpp"
#include "slickdl/line_box.hpp"
#include "slickdl/render_texture.hpp"
#include "stddebug.hpp"

namespace boxes {

using boxes_t = struct boxes {
    boxes() = delete;

    constexpr boxes(
        std::span< std::span< const slickdl::box_t< float > > > _frames )
        : _frames( _frames | std::ranges::to< std::vector< frame_t > >() ) {
        stdfunc::assert( !_frames.empty() );
        stdfunc::assert( std::ranges::none_of(
            _frames,
            []( std::span< const slickdl::box_t< float > > _frame ) -> bool {
                return ( _frame.empty() );
            } ) );
    }

    // TODO: Improve
    constexpr boxes(
        std::initializer_list<
            std::initializer_list< slickdl::box_t< float > > > _frames )
        : _frames( _frames | std::ranges::to< std::vector< frame_t > >() ) {
        stdfunc::assert( !this->_frames.empty() );
        stdfunc::assert( std::ranges::none_of(
            this->_frames, []( const frame_t& _frame ) -> bool {
                return ( _frame.empty() );
            } ) );
    }

    boxes( const boxes& ) = default;
    boxes( boxes&& ) = default;
    ~boxes() = default;

    auto operator=( const boxes& ) -> boxes& = default;
    auto operator=( boxes&& ) -> boxes& = default;

    [[nodiscard]] constexpr auto color() const -> auto& { return ( _color ); }

    [[nodiscard]] constexpr auto currentFrame() const
        -> std::span< const slickdl::box_t< float > > {
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

    void render( slickdl::renderer_t& _renderer,
                 const slickdl::box_t< float >& _screenSpaceTarget,
                 bool _doFill ) const;

private:
    // Frame is a list of boxes
    using frame_t = std::vector< slickdl::box_t< float > >;

    std::vector< frame_t > _frames;
    size_t _currentFrame{};
    slickdl::color_t _color{};
};

} // namespace boxes
