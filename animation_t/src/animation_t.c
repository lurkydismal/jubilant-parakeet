#include "animation_t.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>

#include "stdfunc.h"

animation_t animation_t$create( void ) {
    animation_t l_returnValue = DEFAULT_ANIMATION;

    l_returnValue.keyFrames =
        ( SDL_Texture** )createArray( sizeof( SDL_Texture* ) );
    l_returnValue.frames = ( size_t* )createArray( sizeof( size_t ) );
    l_returnValue.targetBoxes = boxes_t$create();

    return ( l_returnValue );
}

void animation_t$destroy( animation_t* _animation ) {
    SDL_free( _animation->keyFrames );
    SDL_free( _animation->frames );

    boxes_t$destroy( &( _animation->targetBoxes ) );
}

animation_t animation_t$load( SDL_Renderer* _renderer,
                              const char* _path,
                              const char* _pattern ) {
    animation_t l_returnValue = animation_t$create();

    int l_fileCount = 0;

    const char* l_basePath = SDL_GetBasePath();
    char* l_path = duplicateString( _path );

    {
        char** l_files;

        {
            const size_t l_pathLength =
                concatBeforeAndAfterString( &l_path, l_basePath, "/." );

            l_files = SDL_GlobDirectory(
                l_path, _pattern, SDL_GLOB_CASEINSENSITIVE, &l_fileCount );

            trim( l_path, 0, ( l_pathLength - 2 ) );
        }

        {
            char* const* l_filesEnd = ( l_files + l_fileCount );

            preallocateArray( ( void*** )( &( l_returnValue.keyFrames ) ),
                              l_fileCount );

            for ( char** _file = l_files; _file != l_filesEnd; _file++ ) {
                SDL_Log( "Loading file: \"%s\" as BMP\n", *_file );

                const size_t l_indexInTextureArray = ( l_filesEnd - _file );

                {
                    char* l_targetBoxAsStringTrimmedAllocation =
                        duplicateString( *_file );
                    char* l_targetBoxAsStringTrimmed =
                        l_targetBoxAsStringTrimmedAllocation;

                    const size_t l_targetBoxStartIndex =
                        ( findSymbolInString( l_targetBoxAsStringTrimmed,
                                              '_' ) +
                          1 );

                    l_targetBoxAsStringTrimmed += l_targetBoxStartIndex;

                    l_targetBoxAsStringTrimmed[ findSymbolInString(
                        l_targetBoxAsStringTrimmed, 'x' ) ] = ' ';

                    const size_t l_targetBoxEndIndex =
                        findSymbolInString( l_targetBoxAsStringTrimmed, '_' );

                    l_targetBoxAsStringTrimmed[ l_targetBoxEndIndex ] = '\0';

                    char* l_targetBoxAsString =
                        duplicateString( l_targetBoxAsStringTrimmed );

                    SDL_free( l_targetBoxAsStringTrimmedAllocation );

                    concatBeforeAndAfterString( &l_targetBoxAsString, "0 0 ",
                                                " 1-2" );

                    l_returnValue.targetBoxes =
                        boxes_t$load$fromString( l_targetBoxAsString, NULL );

                    SDL_free( l_targetBoxAsString );
                }

                {
                    SDL_Surface* l_fileSufrace;

                    {
                        char* l_filePath = duplicateString( l_path );

                        concatBeforeAndAfterString( &l_filePath, "", *_file );

                        l_fileSufrace = SDL_LoadBMP( l_filePath );

                        SDL_free( l_filePath );
                    }

                    SDL_Texture* l_fileTexture = SDL_CreateTextureFromSurface(
                        _renderer, l_fileSufrace );

                    SDL_DestroySurface( l_fileSufrace );

                    insertIntoArrayByIndex(
                        ( void*** )( &( l_returnValue.keyFrames ) ),
                        l_indexInTextureArray, l_fileTexture );

                    {
                        // Trim filename and extension
                        trim( *_file,
                              ( findLastSymbolInString( *_file, '_' ) + 1 ),
                              findSymbolInString( *_file, '.' ) );

                        char** l_indexStartAndEndAsString =
                            splitStringIntoArray( *_file, "-" );

                        const size_t l_indexStart = SDL_strtoul(
                            l_indexStartAndEndAsString[ 1 ], NULL, 10 );
                        const size_t l_indexEnd = SDL_strtoul(
                            l_indexStartAndEndAsString[ 2 ], NULL, 10 );

                        // Free 2 elements and l_indexStartAndEndAsString
                        {
                            SDL_free( l_indexStartAndEndAsString[ 1 ] );
                            SDL_free( l_indexStartAndEndAsString[ 2 ] );

                            SDL_free( l_indexStartAndEndAsString );
                        }

                        SDL_Log( "Frames from %d to %d\n", l_indexStart,
                                 l_indexEnd );

                        if ( l_indexEnd >
                             arrayLength( l_returnValue.frames ) ) {
                            preallocateArray(
                                ( void*** )( &( l_returnValue.frames ) ),
                                ( l_indexEnd -
                                  arrayLength( l_returnValue.frames ) - 1 ) );
                        }

                        for ( size_t _index = l_indexStart; _index < l_indexEnd;
                              _index++ ) {
                            l_returnValue.frames[ _index ] =
                                l_indexInTextureArray;
                        }
                    }
                }
            }
        }

        SDL_free( l_files );
    }

    SDL_Log( "Loaded %u files from directory \"%s\"\n", l_fileCount, l_path );

EXIT:
    SDL_free( l_path );

    return ( l_returnValue );
}

void animation_t$unload( animation_t* _animation ) {
    if ( arrayLength( _animation->keyFrames ) ) {
        FOR_ARRAY( SDL_Texture**, _animation->keyFrames ) {
            SDL_DestroyTexture( *_element );
        }
    }

    if ( ( _animation->keyFrames ) && ( _animation->frames ) ) {
        animation_t$destroy( _animation );
    }
}

void animation_t$step( animation_t* _animation, bool _canLoop ) {
    if ( _animation->frames ) {
        if ( arrayLength( _animation->frames ) > 1 ) {
            if ( _animation->currentFrame !=
                 arrayLength( _animation->frames ) ) {
                _animation->currentFrame++;

            } else {
                if ( _canLoop ) {
                    _animation->currentFrame = 1;
                }
            }

            boxes_t$step( &( _animation->targetBoxes ), _canLoop );
        }
    }
}

void animation_t$render( SDL_Renderer* _renderer,
                         const animation_t* _animation,
                         const SDL_FRect* _targetRectangle ) {
    const boxes_t* l_targetBoxes = &( _animation->targetBoxes );

    // Always a single box
    const SDL_FRect* l_targetBox =
        l_targetBoxes->keyFrames
            [ l_targetBoxes->frames[ l_targetBoxes->currentFrame ][ 1 ] ];

    const SDL_FRect l_targetRectangle = {
        ( _targetRectangle->x + l_targetBox->x ),
        ( _targetRectangle->y + l_targetBox->y ), l_targetBox->w,
        l_targetBox->h };

    SDL_RenderTexture( _renderer,
                       ( _animation->keyFrames[ (
                           _animation->frames[ _animation->currentFrame ] ) ] ),
                       NULL, &l_targetRectangle );
}
