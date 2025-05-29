#include "boxes_t.h"

#include "log.h"
#include "stdfunc.h"

boxes_t boxes_t$create( void ) {
    boxes_t l_returnValue = DEFAULT_BOXES;

    l_returnValue.keyFrames = createArray( SDL_FRect* );
    l_returnValue.frames = createArray( size_t* );

    return ( l_returnValue );
}

bool boxes_t$destroy( boxes_t* restrict _boxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        goto EXIT;
    }

    {
        FREE_ARRAY( _boxes->keyFrames );

        FREE_ARRAY( _boxes->frames );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool boxes_t$load$one( boxes_t* restrict _boxes,
                       const SDL_FRect* restrict _targetRectangle,
                       size_t _startIndex,
                       size_t _endIndex ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_targetRectangle ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_startIndex ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_endIndex ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( _startIndex > _endIndex ) ) {
        goto EXIT;
    }

    {
        log$transaction$query$format(
            ( logLevel_t )debug,
            "Box properties: X = %f, Y = %f, Width = %f"
            ", Heigth = %f, Start = %zu, End = %zu\n",
            _targetRectangle->x, _targetRectangle->y, _targetRectangle->w,
            _targetRectangle->h, _startIndex, _endIndex );

        // Key frame
        {
            SDL_FRect* l_targetRectanbleAllocated =
                ( SDL_FRect* )malloc( sizeof( SDL_FRect ) );

            __builtin_memcpy( l_targetRectanbleAllocated, _targetRectangle,
                              sizeof( SDL_FRect ) );

            insertIntoArray( &( _boxes->keyFrames ),
                             l_targetRectanbleAllocated );
        }

        // Fill key frame index in frames
        {
            // Allocate frames
            {
                const size_t l_framesLength = arrayLength( _boxes->frames );

                if ( _endIndex > l_framesLength ) {
                    preallocateArray( &( _boxes->frames ),
                                      ( _endIndex - l_framesLength - 1 ) );

                    size_t** l_frameToCreateBegin =
                        &( _boxes->frames[ l_framesLength ] );
                    size_t* const* l_frameToCreateEnd =
                        arrayLastElementPointer( _boxes->frames );

                    for ( size_t** _frameToCreate = l_frameToCreateBegin;
                          _frameToCreate != ( l_frameToCreateEnd + 1 );
                          _frameToCreate++ ) {
                        *_frameToCreate = createArray( size_t );
                    }
                }
            }

            FOR_RANGE( size_t, _startIndex, _endIndex ) {
                insertIntoArray( &( _boxes->frames[ _index - 1 ] ),
                                 ( arrayLength( _boxes->keyFrames ) - 1 ) );
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// X Y Width Height StartIndex-EndIndex
bool boxes_t$load$one$fromString( boxes_t* restrict _boxes,
                                  const char* restrict _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )debug,
                                      "Boxes loading string: '%s'\n", _string );

        char** l_boxProperties = splitStringIntoArrayBySymbol( _string, ' ' );

        // X
        // Y
        // Width
        // Height
        // StartIndex - EndIndex
        if ( UNLIKELY( arrayLength( l_boxProperties ) != 5 ) ) {
            l_returnValue = false;

            goto EXIT_LOADING;
        }

        const float l_x = strtof( l_boxProperties[ 0 ], NULL );
        const float l_y = strtof( l_boxProperties[ 1 ], NULL );
        const float l_width = strtof( l_boxProperties[ 2 ], NULL );
        const float l_height = strtof( l_boxProperties[ 3 ], NULL );

        const SDL_FRect l_targetRectangle = {
            .x = l_x, .y = l_y, .w = l_width, .h = l_height };

        {
            char** l_startAndEndIndexAsString =
                splitStringIntoArrayBySymbol( l_boxProperties[ 4 ], '-' );

            const size_t l_startIndex = strtoul(
                arrayFirstElement( l_startAndEndIndexAsString ), NULL, 10 );
            const size_t l_endIndex = strtoul(
                arrayLastElement( l_startAndEndIndexAsString ), NULL, 10 );

            FREE_ARRAY_ELEMENTS( l_startAndEndIndexAsString );
            FREE_ARRAY( l_startAndEndIndexAsString );

            l_returnValue = boxes_t$load$one( _boxes, &l_targetRectangle,
                                              l_startIndex, l_endIndex );
        }

    EXIT_LOADING:
        FREE_ARRAY_ELEMENTS( l_boxProperties );
        FREE_ARRAY( l_boxProperties );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool boxes_t$load$fromAsset( boxes_t* restrict _boxes,
                             const asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )debug,
                                      "Boxes loading asset: Size = %zu\n",
                                      _asset->size );

        char* l_dataWithNull =
            ( char* )malloc( ( _asset->size + 1 ) * sizeof( char ) );

        __builtin_memcpy( l_dataWithNull, _asset->data, _asset->size );

        l_dataWithNull[ _asset->size ] = '\0';

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Boxes loading asset: Data = '%s'\n",
                                      l_dataWithNull );

        char** l_lines = splitStringIntoArrayBySymbol( l_dataWithNull, '\n' );

        FOR_ARRAY( char* const*, l_lines ) {
            l_returnValue = boxes_t$load$one$fromString( _boxes, *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        FREE_ARRAY_ELEMENTS( l_lines );
        FREE_ARRAY( l_lines );

        free( l_dataWithNull );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// fileName_ColorAsHex.extension
bool boxes_t$load$fromFiles( boxes_t* restrict _boxes,
                             char* const* restrict _files ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_files ) || UNLIKELY( !arrayLength( _files ) ) ) {
        goto EXIT;
    }

    {
        FOR_ARRAY( char* const*, _files ) {
            log$transaction$query$format( ( logLevel_t )debug,
                                          "Loading file: '%s' as boxes_t\n",
                                          *_element );

            asset_t l_asset = asset_t$create();

            l_returnValue = asset_t$load( &l_asset, *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            l_returnValue = boxes_t$load$fromAsset( _boxes, &l_asset );

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

            // Color
            {
                char* l_colorAsString = *_element;

                const ssize_t l_colorAsStringStartIndex =
                    findSymbolInString( l_colorAsString, '_' );

                if ( l_colorAsStringStartIndex == -1 ) {
                    goto EXIT_COLOR_TRIM_FILE_NAME;
                }

                trim( &l_colorAsString, ( l_colorAsStringStartIndex + 1 ),
                      findLastSymbolInString( l_colorAsString, '.' ) );

            EXIT_COLOR_TRIM_FILE_NAME:
                const color_t l_color =
                    color_t$convert$fromString( l_colorAsString );

                _boxes->color = l_color;

                {
                    l_colorAsString = color_t$convert$toString( &l_color );

                    log$transaction$query$format( ( logLevel_t )debug,
                                                  "Box color: '%s'\n",
                                                  l_colorAsString );

                    free( l_colorAsString );
                }
            }
        }

        log$transaction$query$format(
            ( logLevel_t )debug, "Loaded %zu boxes and %zu frames\n",
            arrayLength( _boxes->keyFrames ), arrayLength( _boxes->frames ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool boxes_t$unload( boxes_t* restrict _boxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        goto EXIT;
    }

    {
        FREE_ARRAY_ELEMENTS( _boxes->keyFrames );

        FOR_ARRAY( size_t* const*, _boxes->frames ) {
            FREE_ARRAY( *_element );
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool boxes_t$step( boxes_t* restrict _boxes, bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        goto EXIT;
    }

    {
        if ( _boxes->frames ) {
            if ( arrayLength( _boxes->frames ) > 1 ) {
                if ( _boxes->currentFrame != arrayLength( _boxes->frames ) ) {
                    _boxes->currentFrame++;

                } else {
                    if ( _canLoop ) {
                        _boxes->currentFrame = 0;
                    }
                }
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool boxes_t$render( const boxes_t* restrict _boxes,
                     SDL_Renderer* _renderer,
                     const SDL_FRect* restrict _targetRectangle,
                     bool _doFill ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_targetRectangle ) ) {
        goto EXIT;
    }

    {
        const size_t* l_boxesIndexes =
            ( _boxes->frames[ _boxes->currentFrame ] );

        color_t l_colorBefore;

        l_returnValue = SDL_GetRenderDrawColor(
            _renderer, &l_colorBefore.red, &l_colorBefore.green,
            &l_colorBefore.blue, &l_colorBefore.alpha );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Getting renderer draw color: '%s'\n",
                                          SDL_GetError() );

            goto EXIT;
        }

        l_returnValue = SDL_SetRenderDrawColor(
            _renderer, _boxes->color.red, _boxes->color.green,
            _boxes->color.blue, _boxes->color.alpha );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Setting renderer draw color: '%s'\n",
                                          SDL_GetError() );

            goto EXIT;
        }

        FOR_ARRAY( const size_t*, l_boxesIndexes ) {
            const SDL_FRect* l_boxRectangle = _boxes->keyFrames[ *_element ];
            const SDL_FRect l_targetRectangle = {
                .x = ( _targetRectangle->x + l_boxRectangle->x ),
                .y = ( _targetRectangle->y + l_boxRectangle->y ),
                .w = l_boxRectangle->w,
                .h = l_boxRectangle->h };

            if ( _doFill ) {
                l_returnValue =
                    SDL_RenderFillRect( _renderer, &l_targetRectangle );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error, "Render filled rectangle: '%s'\n",
                        SDL_GetError() );

                    goto EXIT;
                }

            } else {
                l_returnValue = SDL_RenderRect( _renderer, &l_targetRectangle );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query$format( ( logLevel_t )error,
                                                  "Render rectangle: '%s'\n",
                                                  SDL_GetError() );

                    goto EXIT;
                }
            }
        }

        l_returnValue = SDL_SetRenderDrawColor(
            _renderer, l_colorBefore.red, l_colorBefore.green,
            l_colorBefore.blue, l_colorBefore.alpha );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Setting renderer draw color: '%s'\n",
                                          SDL_GetError() );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
