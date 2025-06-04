#include "animation_t.h"

#include <SDL3/SDL_rect.h>
#include <SDL3_image/SDL_image.h>

#include "log.h"
#include "stdfunc.h"

animation_t animation_t$create( void ) {
    animation_t l_returnValue = DEFAULT_ANIMATION;

    l_returnValue.keyFrames = createArray( SDL_Texture* );
    l_returnValue.frames = createArray( size_t );
    l_returnValue.targetBoxes = boxes_t$create();

    return ( l_returnValue );
}

bool animation_t$destroy( animation_t* restrict _animation ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_animation ) ) {
        goto EXIT;
    }

    {
        FREE_ARRAY( _animation->keyFrames );

        _animation->keyFrames = NULL;

        FREE_ARRAY( _animation->frames );

        _animation->frames = NULL;

        l_returnValue = boxes_t$destroy( &( _animation->targetBoxes ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool animation_t$load$fromAsset( animation_t* restrict _animation,
                                 SDL_Renderer* _renderer,
                                 const asset_t* restrict _asset,
                                 const SDL_FRect* restrict _targetRectangle,
                                 size_t _startIndex,
                                 size_t _endIndex ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_animation ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_targetRectangle ) ) {
        goto EXIT;
    }

    {
        {
#if defined( LOG_ANIMATION )

            // Properties will be printed in boxes_t$load$one
            log$transaction$query$format( ( logLevel_t )debug,
                                          "Animation properties: Size = %zu\n",
                                          _asset->size );

#endif

            // Key frame
            {
                SDL_IOStream* l_stream =
                    SDL_IOFromConstMem( _asset->data, _asset->size );

                if ( UNLIKELY( !l_stream ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading animation from asset\n" );

                    goto EXIT;
                }

                SDL_Texture* l_texture =
                    IMG_LoadTexture_IO( _renderer, l_stream, true );

                if ( UNLIKELY( !l_texture ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Creating texture from asset\n" );

                    goto EXIT;
                }

                insertIntoArray( &( _animation->keyFrames ), l_texture );
            }

            // Fill key frame index in frames
            FOR_RANGE( size_t, _startIndex, _endIndex ) {
                insertIntoArray( &( _animation->frames ),
                                 ( arrayLength( _animation->keyFrames ) - 1 ) );
            }
        }

        l_returnValue =
            boxes_t$load$one( &( _animation->targetBoxes ), _targetRectangle,
                              _startIndex, _endIndex );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// fileName_WidthxHeight_StartIndex-EndIndex.extension
bool animation_t$load$fromFiles( animation_t* restrict _animation,
                                 SDL_Renderer* _renderer,
                                 char* const* restrict _files ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_animation ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_files ) || UNLIKELY( !arrayLength( _files ) ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;

        FOR_ARRAY( char* const*, _files ) {
#if defined( LOG_ANIMATION )

            log$transaction$query$format( ( logLevel_t )debug,
                                          "Loading file: '%s' as animation_t\n",
                                          *_element );

#endif

            char** l_animationProperties =
                splitStringIntoArrayBySymbol( *_element, '_' );

            // File Name
            // Width x Height
            // StartIndex - EndIndex
            if ( UNLIKELY( arrayLength( l_animationProperties ) != 3 ) ) {
                l_returnValue = false;

                goto LOOP_CONTINUE;
            }

            SDL_FRect l_targetRectangle = {
                .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f };

            // Target rectangle
            {
                char* l_targetRectangleSizeAsString =
                    l_animationProperties[ 1 ];

                {
                    char** l_targetRectangleWidthAndHeight =
                        splitStringIntoArrayBySymbol(
                            l_targetRectangleSizeAsString, 'x' );

                    l_targetRectangle.w = strtof(
                        arrayFirstElement( l_targetRectangleWidthAndHeight ),
                        NULL );
                    l_targetRectangle.h = strtof(
                        arrayLastElement( l_targetRectangleWidthAndHeight ),
                        NULL );

                    FREE_ARRAY_ELEMENTS( l_targetRectangleWidthAndHeight );
                    FREE_ARRAY( l_targetRectangleWidthAndHeight );
                }
            }

            size_t l_startIndex = 0;
            size_t l_endIndex = 0;

            // Start and End indexes
            {
                char** l_startAndEndIndexAsString =
                    splitStringIntoArray( l_animationProperties[ 2 ], "-" );

                l_startIndex = strtoul(
                    arrayFirstElement( l_startAndEndIndexAsString ), NULL, 10 );
                l_endIndex = strtoul(
                    arrayLastElement( l_startAndEndIndexAsString ), NULL, 10 );

                FREE_ARRAY_ELEMENTS( l_startAndEndIndexAsString );
                FREE_ARRAY( l_startAndEndIndexAsString );
            }

            {
                asset_t l_asset = asset_t$create();

                l_returnValue = asset_t$load( &l_asset, *_element );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }

                l_returnValue = animation_t$load$fromAsset(
                    _animation, _renderer, &l_asset, &l_targetRectangle,
                    l_startIndex, l_endIndex );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }

                l_returnValue = asset_t$unload( &l_asset );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }

                l_returnValue = asset_t$destroy( &l_asset );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }
            }

        LOOP_CONTINUE:
            FREE_ARRAY_ELEMENTS( l_animationProperties );
            FREE_ARRAY( l_animationProperties );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

#if defined( LOG_ANIMATION )

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Loaded %zu files and %zu frames\n",
                                      arrayLength( _animation->keyFrames ),
                                      arrayLength( _animation->frames ) );

#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool animation_t$unload( animation_t* restrict _animation ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_animation ) ) {
        goto EXIT;
    }

    {
        FOR_ARRAY( SDL_Texture**, _animation->keyFrames ) {
            SDL_DestroyTexture( *_element );
        }

        l_returnValue = boxes_t$unload( &( _animation->targetBoxes ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool animation_t$step( animation_t* restrict _animation, bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_animation ) ) {
        goto EXIT;
    }

    {
        if ( _animation->frames ) {
            if ( arrayLength( _animation->frames ) > 1 ) {
                if ( _animation->currentFrame !=
                     arrayLength( _animation->frames ) ) {
                    _animation->currentFrame++;

                } else {
                    if ( _canLoop ) {
                        _animation->currentFrame = 0;
                    }
                }

                l_returnValue =
                    boxes_t$step( &( _animation->targetBoxes ), _canLoop );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool animation_t$render( const animation_t* restrict _animation,
                         SDL_Renderer* _renderer,
                         const SDL_FRect* restrict _targetRectangle ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_animation ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_targetRectangle ) ) {
        goto EXIT;
    }

    {
        const boxes_t* l_targetBoxes = &( _animation->targetBoxes );

        // Always a single box
        const SDL_FRect* l_animationFrameTargetRectangle =
            l_targetBoxes->keyFrames[ arrayFirstElement(
                l_targetBoxes->frames[ l_targetBoxes->currentFrame ] ) ];

        const SDL_FRect l_targetRectangle = {
            _targetRectangle->x, _targetRectangle->y,
            l_animationFrameTargetRectangle->w,
            l_animationFrameTargetRectangle->h };

        l_returnValue = SDL_RenderTexture(
            _renderer,
            _animation
                ->keyFrames[ _animation->frames[ _animation->currentFrame ] ],
            NULL, &l_targetRectangle );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Rendering texture: '%s'\n",
                                          SDL_GetError() );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
