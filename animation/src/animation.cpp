#include "animation.hpp"

#include "slickdl/line_box.hpp"

namespace animation {

namespace {

inline auto resolveTargetBox(
    const slickdl::box_t< float >& _targetBoxSizes,
    const slickdl::box_t< float >& _targetBoxCoordinates )
    -> slickdl::box_t< float > {
    return ( slickdl::box_t< float >{
        _targetBoxCoordinates.x,
        _targetBoxCoordinates.y,
        _targetBoxSizes.width,
        _targetBoxSizes.height,
    } );
}

} // namespace

void animation_t::render(
    slickdl::renderer_t& _renderer,
    const slickdl::box_t< float >& _targetBoxCoordinates ) const {
    currentKeyFrame().render(
        _renderer,
        resolveTargetBox( currentTargetBox(), _targetBoxCoordinates ) );
}

void animation_t::render(
    slickdl::renderer_t& _renderer,
    const slickdl::box_t< float >& _targetBoxCoordinates,
    double _angle,
    slickdl::flip_t _flipMode,
    const std::optional< slickdl::point_t< float > >& _center ) const {
    currentKeyFrame().renderRotated(
        _renderer, _angle,
        resolveTargetBox( currentTargetBox(), _targetBoxCoordinates ),
        _flipMode, _center );
}

} // namespace animation

#if 0
#include <SDL3_image/SDL_image.h>

bool animation_t$load$fromAsset( animation_t* _animation,
                                 SDL_Renderer* _renderer,
                                 const asset_t* _asset,
                                 const SDL_FRect* _targetRectangle,
                                 size_t _startIndex,
                                 size_t _endIndex ) {
    {
        {
#if defined( LOG_ANIMATION )

            // Properties will be printed in boxes_t$load$one
            log$transaction$query$format( ( logLevel_t )debug,
                                          "Animation properties: Size = %zu",
                                          _asset->size );

#endif

            size_t l_keyFrameIndex = 0;

            // Key frame
            {
                SDL_IOStream* l_stream =
                    SDL_IOFromConstMem( _asset->data, _asset->size );

                l_returnValue = !!( l_stream );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading animation from asset" );

                    goto EXIT;
                }

                SDL_Texture* l_texture =
                    IMG_LoadTexture_IO( _renderer, l_stream, true );

                l_returnValue = !!( l_texture );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Creating texture from asset" );

                    if ( UNLIKELY( SDL_CloseIO( l_stream ) ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Closing IO stream" );
                    }

                    goto EXIT;
                }

                l_keyFrameIndex =
                    insertIntoArray( &( _animation->keyFrames ), l_texture );
            }

            // Preallocate frames
            {
                const arrayLength_t l_framesAmount =
                    arrayLength( _animation->frames );

                if ( LIKELY( _endIndex >= l_framesAmount ) ) {
                    int64_t l_preallocationAmount =
                        ( _endIndex - l_framesAmount );

                    preallocateArray( &( _animation->frames ),
                                      l_preallocationAmount );
                }
            }

            // Fill key frame index in frames
            FOR_RANGE( size_t, _startIndex, ( _endIndex + 1 ) ) {
                _animation->frames[ _index - 1 ] = l_keyFrameIndex;
            }
        }

        l_returnValue =
            boxes_t$load$one( &( _animation->targetBoxes ), _targetRectangle,
                              _startIndex, _endIndex );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Loading target boxes" );

            goto EXIT;
        }
    }
}
#endif
