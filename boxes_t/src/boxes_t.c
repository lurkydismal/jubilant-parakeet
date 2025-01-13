#include "boxes_t.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

#include "stdfunc.h"

boxes_t boxes_t$create( void ) {
    boxes_t l_returnValue = DEFAULT_BOXES;

    l_returnValue.keyFrames =
        ( SDL_FRect** )createArray( sizeof( SDL_FRect* ) );
    l_returnValue.frames = ( size_t** )createArray( sizeof( size_t* ) );

    return ( l_returnValue );
}

void boxes_t$destroy( boxes_t* _boxes ) {
    FREE_ARRAY( SDL_FRect**, _boxes->keyFrames );
    FREE_ARRAY( size_t**, _boxes->frames );
}

boxes_t boxes_t$load$fromString( const char* _string,
                                 const char* _boxesColorAsString ) {
    boxes_t l_returnValue = boxes_t$create();

    if ( _boxesColorAsString ) {
        l_returnValue.color = color_t$fromString( _boxesColorAsString, 16 );
    }

    {
        char** l_boxesStrings = splitStringIntoArray( _string, "\n" );
        const size_t l_keyFramesCount = arrayLength( l_boxesStrings );

        preallocateArray( ( void*** )( &( l_returnValue.keyFrames ) ),
                          l_keyFramesCount );

        FOR_ARRAY( char* const*, l_boxesStrings ) {
            const size_t l_indexInBoxesArray =
                ( arrayLastElementPointer( l_boxesStrings ) - _element + 1 );

            {
                char** l_boxProperties = splitStringIntoArray( *_element, " " );

                {
                    const float l_x = SDL_atof( l_boxProperties[ 1 ] );
                    const float l_y = SDL_atof( l_boxProperties[ 2 ] );
                    const float l_width = SDL_atof( l_boxProperties[ 3 ] );
                    const float l_height = SDL_atof( l_boxProperties[ 4 ] );

                    SDL_Log(
                        "Box properties: X = %f, Y = %f, Width = %f"
                        ", Heigth = %f\n",
                        l_x, l_y, l_width, l_height );

                    const SDL_FRect l_box = {
                        .x = l_x, .y = l_y, .w = l_width, .h = l_height };

                    SDL_FRect* l_boxAllocated =
                        ( SDL_FRect* )SDL_malloc( sizeof( SDL_FRect ) );

                    SDL_memcpy( l_boxAllocated, &l_box, sizeof( SDL_FRect ) );

                    insertIntoArrayByIndex(
                        ( void*** )( &( l_returnValue.keyFrames ) ),
                        l_indexInBoxesArray, l_boxAllocated );
                }

                {
                    char** l_indexStartAndEndAsString =
                        splitStringIntoArray( l_boxProperties[ 5 ], "-" );

                    const size_t l_indexStart = SDL_strtoul(
                        l_indexStartAndEndAsString[ 1 ], NULL, 10 );
                    const size_t l_indexEnd = SDL_strtoul(
                        l_indexStartAndEndAsString[ 2 ], NULL, 10 );

                    // Free 2 elements and
                    // l_indexStartAndEndAsString
                    {
                        SDL_free( l_indexStartAndEndAsString[ 1 ] );
                        SDL_free( l_indexStartAndEndAsString[ 2 ] );

                        SDL_free( l_indexStartAndEndAsString );
                    }

                    SDL_Log( "Frames from %d to %d\n", l_indexStart,
                             l_indexEnd );

                    const size_t l_framesLength =
                        arrayLength( l_returnValue.frames );

                    if ( l_indexEnd > l_framesLength ) {
                        preallocateArray(
                            ( void*** )( &( l_returnValue.frames ) ),
                            ( l_indexEnd - l_framesLength - 1 ) );

                        size_t** l_frameToCreateBegin =
                            ( arrayFirstElementPointer( l_returnValue.frames ) +
                              l_framesLength );
                        size_t* const* l_frameToCreateEnd =
                            arrayLastElementPointer( l_returnValue.frames );

                        for ( size_t** _frameToCreate = l_frameToCreateBegin;
                              _frameToCreate != ( l_frameToCreateEnd + 1 );
                              _frameToCreate++ ) {
                            *_frameToCreate =
                                ( size_t* )createArray( sizeof( size_t* ) );
                        }
                    }

                    for ( size_t _index = l_indexStart; _index < l_indexEnd;
                          _index++ ) {
                        insertIntoArray(
                            ( void*** )( &( l_returnValue.frames[ _index ] ) ),
                            ( void* )l_indexInBoxesArray );
                    }
                }

                FREE_ARRAY( char**, l_boxProperties );
            }
        }

        FREE_ARRAY( char**, l_boxesStrings );
    }

    return ( l_returnValue );
}

boxes_t boxes_t$load( const char* _path, const char* _pattern ) {
    boxes_t l_returnValue = DEFAULT_BOXES;

    const char* l_basePath = SDL_GetBasePath();
    char* l_path = duplicateString( _path );

    {
        int l_fileCount = 0;
        char** l_files;

        {
            const size_t l_pathLength =
                concatBeforeAndAfterString( &l_path, l_basePath, "/." );

            l_files = SDL_GlobDirectory(
                l_path, _pattern, SDL_GLOB_CASEINSENSITIVE, &l_fileCount );

            // Remove /.
            trim( l_path, 0, ( l_pathLength - 2 ) );
        }

        if ( !l_fileCount ) {
            goto EXIT_PROCESSING_FILES;
        }

        {
            SDL_Log( "Loading file: \"%s\" as boxes_t\n", l_path );

            // All boxes always in a single file
            concatBeforeAndAfterString( &l_path, "", l_files[ 0 ] );

            char* l_colorString;
            char* l_colorStringTrimmed;

            // Color
            {
                l_colorString = duplicateString( l_path );
                l_colorStringTrimmed = l_colorString;

                trim( l_colorStringTrimmed,
                      ( findSymbolInString( l_colorString, '_' ) + 1 ),
                      findLastSymbolInString( l_colorString, '.' ) );
            }

            {
                char* l_fileContent = SDL_LoadFile( l_path, NULL );

                l_returnValue = boxes_t$load$fromString( l_fileContent,
                                                         l_colorStringTrimmed );

                SDL_free( l_fileContent );
            }

            SDL_free( l_colorString );
        }

    EXIT_PROCESSING_FILES:
        SDL_free( l_files );
    }

    size_t l_boxCount = 0;

    if ( l_returnValue.keyFrames ) {
        l_boxCount = arrayLength( l_returnValue.keyFrames );
    }

    SDL_Log( "Loaded %u boxes from file \"%s\"\n", l_boxCount, l_path );

EXIT:
    SDL_free( l_path );

    if ( !l_returnValue.frames ) {
        l_returnValue = boxes_t$create();
    }

    return ( l_returnValue );
}

void boxes_t$unload( boxes_t* _boxes ) {
    if ( ( _boxes->keyFrames ) && ( _boxes->frames ) ) {
        boxes_t$destroy( _boxes );
    }
}

void boxes_t$step( boxes_t* _boxes, bool _canLoop ) {
    if ( _boxes->frames ) {
        if ( arrayLength( _boxes->frames ) > 1 ) {
            if ( _boxes->currentFrame != arrayLength( _boxes->frames ) ) {
                _boxes->currentFrame++;

            } else {
                if ( _canLoop ) {
                    _boxes->currentFrame = 1;
                }
            }
        }
    }
}

void boxes_t$render( SDL_Renderer* _renderer,
                     const boxes_t* _boxes,
                     const SDL_FRect* _targetRectanble,
                     bool _doFill ) {
    const size_t* l_boxesIndexes = ( _boxes->frames[ _boxes->currentFrame ] );
    const size_t l_boxesCount = arrayLength( l_boxesIndexes );

    color_t l_colorBefore;

    SDL_GetRenderDrawColor( _renderer, &l_colorBefore.red, &l_colorBefore.green,
                            &l_colorBefore.blue, &l_colorBefore.alpha );
    SDL_SetRenderDrawColor( _renderer, _boxes->color.red, _boxes->color.green,
                            _boxes->color.blue, _boxes->color.alpha );

    FOR_ARRAY( const size_t*, l_boxesIndexes ) {
        const SDL_FRect* l_boxRectangle = _boxes->keyFrames[ *_element ];
        const SDL_FRect l_targetRectangle = {
            .x = ( _targetRectanble->x + l_boxRectangle->x ),
            .y = ( _targetRectanble->y + l_boxRectangle->y ),
            .w = l_boxRectangle->w,
            .h = l_boxRectangle->h };

        if ( _doFill ) {
            SDL_RenderFillRect( _renderer, &l_targetRectangle );

        } else {
            SDL_RenderRect( _renderer, &l_targetRectangle );
        }
    }

    SDL_SetRenderDrawColor( _renderer, l_colorBefore.red, l_colorBefore.green,
                            l_colorBefore.blue, l_colorBefore.alpha );
}
