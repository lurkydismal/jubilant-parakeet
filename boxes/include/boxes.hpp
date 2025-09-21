#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <algorithm>
#include <ranges>

#include "log.hpp"
#include "stdfunc.hpp"

#define BOXES_FILE_EXTENSION "boxes"

namespace boxes {

using box_t = struct box {
    box( float _x,
         float _y,
         float _width,
         float _height,
         size_t _startIndex,
         size_t _endIndex )
        : x( _x ),
          y( _y ),
          width( _width ),
          height( _height ),
          startIndex( _startIndex ),
          endIndex( _endIndex ) {
        stdfunc::assert( !_startIndex );
        stdfunc::assert( !_endIndex );
        stdfunc::assert( _startIndex > _endIndex );

        logg::debug(
            "Box properties: X = {}, Y = {}, Width = {}"
            ", Heigth = {}, Start = {}, End = {}",
            _x, _y, _width, _height, _startIndex, _endIndex );
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
    size_t startIndex;
    size_t endIndex;
};

using boxes_t = struct boxes {
    boxes() = delete;
    boxes( const boxes& ) = default;
    boxes( boxes&& ) = default;
    ~boxes() = default;

    boxes( std::span< std::span< const box_t > > _boxesKeyFrames ) {
        _keyFrames = _boxesKeyFrames |
                     std::ranges::to< std::vector< std::vector< box_t > > >();

        // Generate frames
        {
            size_t l_latestFrameIndex = 0;

            for ( std::span< const box_t > _boxes : _boxesKeyFrames ) {
                for ( const box_t& _box : _boxes ) {
                    const size_t l_endIndex = _box.endIndex;

                    if ( l_endIndex > l_latestFrameIndex ) {
                        l_latestFrameIndex = l_endIndex;
                    }
                }
            }
        }

        // Fill key frame index in frames
        {
            // Preallocate frames
            const auto l_x = [ & ]( const box_t& _box ) -> void {
                const size_t l_framesAmount = _frames.size();

                if ( _box.endIndex >= l_framesAmount ) [[likely]] {
                    int64_t l_preallocationAmount =
                        ( _box.endIndex - l_framesAmount );

                    _frames.reserve( l_preallocationAmount );

                    for ( size_t _frameToCreate : _frames[ l_framesAmount ] ) {
                        *_frameToCreate = createArray( size_t );
                    }
                }
            };

            // Fill key frame index in frames
            for ( size_t _index :
                  std::views::iota( _box.startIndex, ( _box.endIndex + 1 ) ) ) {
                _frames[ _index - 1 ] = l_keyFrameIndex;
            }
        }
    }

    auto operator=( const boxes& ) -> boxes& = default;
    auto operator=( boxes&& ) -> boxes& = default;

    constexpr auto color() -> auto& { return ( _color ); }

    constexpr auto currentKeyFrame() {
        return ( _frames[ _currentFrame ] |
                 std::views::transform(
                     [ & ]( size_t _index ) -> std::vector< box_t > {
                         return ( _keyFrames[ _index ] );
                     } ) );
    }

    constexpr void step( bool _canLoop ) {
        if ( _currentFrame < ( _frames.size() - 1 ) ) {
            _currentFrame++;

        } else {
            if ( _canLoop ) {
                _currentFrame = 0;
            }
        }
    }

    constexpr auto render( SDL_Renderer* _renderer,
                           const box_t& _screenSpaceTarget,
                           bool _doFill ) -> bool;

private:
    // Key frame is a list of boxes
    using keyFrame_t = std::vector< box_t >;

    // Frame is a list of indexes in key frames
    using frame_t = std::vector< size_t >;

    std::vector< keyFrame_t > _keyFrames;
    std::vector< frame_t > _frames;
    size_t _currentFrame{};
    uint32_t _color{};
};

} // namespace boxes
