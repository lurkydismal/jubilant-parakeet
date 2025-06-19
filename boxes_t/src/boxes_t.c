#include "boxes_t.h"

#include "log.h"
#include "stdfunc.h"

boxes_t boxes_t$create( void ) {
    boxes_t l_returnValue = DEFAULT_BOXES;

    {
        l_returnValue.keyFrames = createArray( SDL_FRect* );
        l_returnValue.frames = createArray( size_t* );
    }

    return ( l_returnValue );
}

bool boxes_t$destroy( boxes_t* restrict _boxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        FREE_ARRAY( _boxes->keyFrames );
        _boxes->keyFrames = NULL;

        FREE_ARRAY( _boxes->frames );
        _boxes->frames = NULL;

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_targetRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_startIndex ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_endIndex ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( _startIndex > _endIndex ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#if defined( LOG_BOXES )

        log$transaction$query$format(
            ( logLevel_t )debug,
            "Box properties: X = %f, Y = %f, Width = %f"
            ", Heigth = %f, Start = %zu, End = %zu",
            _targetRectangle->x, _targetRectangle->y, _targetRectangle->w,
            _targetRectangle->h, _startIndex, _endIndex );

#endif

        // TODO: Improve
        _endIndex++;

        // Key frame
        size_t l_keyFrameIndex = insertIntoArray(
            &( _boxes->keyFrames ), clone( ( SDL_FRect* )_targetRectangle ) );

        // Fill key frame index in frames
        {
            // Preallocate frames
            {
                const arrayLength_t l_framesAmount =
                    arrayLength( _boxes->frames );

                if ( LIKELY( _endIndex > l_framesAmount ) ) {
                    int64_t l_preallocationAmount =
                        ( _endIndex - l_framesAmount - 1 );

                    preallocateArray( &( _boxes->frames ),
                                      l_preallocationAmount );

                    size_t** l_frameToCreateBegin =
                        &( _boxes->frames[ l_framesAmount ] );
                    size_t* const* l_frameToCreateEnd =
                        arrayLastElementPointer( _boxes->frames );

                    for ( size_t** _frameToCreate = l_frameToCreateBegin;
                          _frameToCreate != ( l_frameToCreateEnd + 1 );
                          _frameToCreate++ ) {
                        *_frameToCreate = createArray( size_t );
                    }
                }
            }

            // Fill key frame index in frames
            FOR_RANGE( size_t, _startIndex, _endIndex ) {
                insertIntoArray( &( _boxes->frames[ _index - 1 ] ),
                                 l_keyFrameIndex );
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// X Y Width Height StartIndex-EndIndex
bool boxes_t$load$one$fromString( boxes_t* restrict _boxes,
                                  char* restrict _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_string ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#if defined( LOG_BOXES )

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Boxes loading string: '%s'", _string );

#endif

        // Trim to comment symbol
        {
            char* l_symbol = _string;

            while ( *l_symbol ) {
                if ( *l_symbol == COMMENT_SYMBOL ) {
                    *l_symbol = '\0';

                    break;
                }

                l_symbol++;
            }

            l_returnValue = !!( __builtin_strlen( _string ) );

            if ( !l_returnValue ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Empty input after comment trim" );

                goto EXIT;
            }
        }

        {
            char** l_boxProperties =
                splitStringIntoArrayBySymbol( _string, ' ' );

            // X
            // Y
            // Width
            // Height
            // StartIndex - EndIndex
            l_returnValue = ( arrayLength( l_boxProperties ) == 5 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Expected 5 box properties, got %zu",
                    arrayLength( l_boxProperties ) );

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

                l_returnValue =
                    ( arrayLength( l_startAndEndIndexAsString ) == 2 );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error,
                        "Invalid index format, expected Start-End got '%s'",
                        l_boxProperties[ 4 ] );

                    goto EXIT_LOADING2;
                }

                const size_t l_startIndex = strtoul(
                    arrayFirstElement( l_startAndEndIndexAsString ), NULL, 10 );
                const size_t l_endIndex = strtoul(
                    arrayLastElement( l_startAndEndIndexAsString ), NULL, 10 );

                l_returnValue = boxes_t$load$one( _boxes, &l_targetRectangle,
                                                  l_startIndex, l_endIndex );

            EXIT_LOADING2:
                FREE_ARRAY_ELEMENTS( l_startAndEndIndexAsString );
                FREE_ARRAY( l_startAndEndIndexAsString );
            }

        EXIT_LOADING:
            FREE_ARRAY_ELEMENTS( l_boxProperties );
            FREE_ARRAY( l_boxProperties );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Loading box" );

                goto EXIT;
            }
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
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#if defined( LOG_BOXES )

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Boxes loading asset: Size = %zu",
                                      _asset->size );

#endif

        char* l_dataWithNull =
            ( char* )malloc( ( _asset->size + 1 ) * sizeof( char ) );

        __builtin_memcpy( l_dataWithNull, _asset->data, _asset->size );

        l_dataWithNull[ _asset->size ] = '\0';

#if defined( LOG_BOXES )

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Boxes loading asset: Data = '%s'",
                                      l_dataWithNull );

#endif

        char** l_lines = splitStringIntoArrayBySymbol( l_dataWithNull, '\n' );

        free( l_dataWithNull );

        FOR_ARRAY( char* const*, l_lines ) {
            l_returnValue = boxes_t$load$one$fromString( _boxes, *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Loading box from string: '%s'",
                                              *_element );

                goto EXIT_LOADING;
            }
        }

        l_returnValue = true;

    EXIT_LOADING:
        FREE_ARRAY_ELEMENTS( l_lines );
        FREE_ARRAY( l_lines );
    }

EXIT:
    return ( l_returnValue );
}

// fileName_ColorAsHex.extension
bool boxes_t$load$fromPaths( boxes_t* restrict _boxes,
                             char* const* restrict _paths ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_paths ) || UNLIKELY( !arrayLength( _paths ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        FOR_ARRAY( char* const*, _paths ) {
#if defined( LOG_BOXES )

            log$transaction$query$format( ( logLevel_t )debug,
                                          "Loading file: '%s' as boxes_t",
                                          *_element );

#endif

            asset_t l_asset = asset_t$create();

            l_returnValue = asset_t$load$fromPath( &l_asset, *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading asset from path" );

                goto EXIT;
            }

            l_returnValue = boxes_t$load$fromAsset( _boxes, &l_asset );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading boxes from asset" );

                goto EXIT;
            }

            l_returnValue = asset_t$unload( &l_asset );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Unloading asset" );

                goto EXIT;
            }

            l_returnValue = asset_t$destroy( &l_asset );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying asset" );

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

#if defined( LOG_BOXES )

                {
                    const char* l_colorAsString =
                        color_t$convert$toStaticString( &l_color );

                    log$transaction$query$format( ( logLevel_t )debug,
                                                  "Box color: '%s'",
                                                  l_colorAsString );
                }

#endif
            }
        }

#if defined( LOG_BOXES )

        log$transaction$query$format(
            ( logLevel_t )debug, "Loaded %zu boxes and %zu frames",
            arrayLength( _boxes->keyFrames ), arrayLength( _boxes->frames ) );

#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool boxes_t$load$fromGlob( boxes_t* restrict _boxes,
                            const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        char** l_paths = getPathsByGlob( _glob, NULL, false );

        l_returnValue = boxes_t$load$fromPaths( _boxes, l_paths );

        FREE_ARRAY_ELEMENTS( l_paths );
        FREE_ARRAY( l_paths );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format(
                ( logLevel_t )error, "Failed loading from glob: '%s'", _glob );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool boxes_t$unload( boxes_t* restrict _boxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        if ( _boxes->frames ) {
            if ( arrayLength( _boxes->frames ) > 1 ) {
                if ( _boxes->currentFrame <
                     ( arrayLength( _boxes->frames ) - 1 ) ) {
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
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_targetRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        color_t l_colorBefore;

        l_returnValue = SDL_GetRenderDrawColor(
            _renderer, &l_colorBefore.red, &l_colorBefore.green,
            &l_colorBefore.blue, &l_colorBefore.alpha );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Getting renderer draw color: '%s'",
                                          SDL_GetError() );

            goto EXIT;
        }

        l_returnValue = SDL_SetRenderDrawColor(
            _renderer, _boxes->color.red, _boxes->color.green,
            _boxes->color.blue, _boxes->color.alpha );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Setting renderer draw color: '%s'",
                                          SDL_GetError() );

            goto EXIT;
        }

        l_returnValue =
            ( _boxes->currentFrame < arrayLength( _boxes->frames ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Invalid boxex current frame" );

            goto EXIT;
        }

        const size_t* l_boxesIndexes =
            ( _boxes->frames[ _boxes->currentFrame ] );

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
                        ( logLevel_t )error, "Render filled rectangle: '%s'",
                        SDL_GetError() );

                    goto EXIT;
                }

            } else {
                l_returnValue = SDL_RenderRect( _renderer, &l_targetRectangle );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query$format( ( logLevel_t )error,
                                                  "Render rectangle: '%s'",
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
                                          "Setting renderer draw color: '%s'",
                                          SDL_GetError() );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
