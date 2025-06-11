#include "background_t.h"

#include "log.h"
#include "stdfunc.h"

background_t background_t$create( void ) {
    background_t l_returnValue = DEFAULT_BACKGROUND;

    {
        l_returnValue.object = object_t$create();
    }

    return ( l_returnValue );
}

bool background_t$destroy( background_t* restrict _background ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) ) {
        goto EXIT;
    }

    {
        l_returnValue = object_t$destroy( &( _background->object ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        free( _background->name );
        free( _background->folder );
        free( _background->extension );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
bool background_t$load( background_t* restrict _background,
                        SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) || UNLIKELY( !_background->name ) ||
         UNLIKELY( !_background->folder ) ||
         UNLIKELY( !_background->extension ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )info,
                                      "Loading background: '%s'\n",
                                      _background->name );

        // TODO: Improve
        {
            const char* l_folder = _background->folder;

            char* l_boxesPath = NULL;

            // Boxes
            {
                l_boxesPath = duplicateString( l_folder );

                concatBeforeAndAfterString( &l_boxesPath, "/", ".boxes" );
                concatBeforeAndAfterString( &l_boxesPath, l_folder, NULL );
            }

            char** l_animation = NULL;

            // Animation
            {
                char* l_glob = duplicateString( "*." );

                concatBeforeAndAfterString( &l_glob, l_folder,
                                            _background->extension );

                char* l_directory = duplicateString( l_folder );

                concatBeforeAndAfterString(
                    &l_directory, asset_t$loader$assetsDirectory$get(), "/" );

                l_animation = getPathsByGlob( l_glob, l_directory );

                free( l_glob );
                free( l_directory );

                FOR_ARRAY( char**, l_animation ) {
                    concatBeforeAndAfterString( _element, "/", NULL );
                    concatBeforeAndAfterString( _element, l_folder, NULL );
                }
            }

            l_returnValue = object_t$state$add$fromFiles(
                &( _background->object ), _renderer, l_boxesPath, l_animation,
                false, true );

            free( l_boxesPath );

            FREE_ARRAY_ELEMENTS( l_animation );
            FREE_ARRAY( l_animation );
        }

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        // Background always have only single state
        _background->object.currentState =
            arrayFirstElement( _background->object.states );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool background_t$unload( background_t* restrict _background ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) ) {
        goto EXIT;
    }

    {
        l_returnValue = object_t$states$remove( &( _background->object ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool background_t$step( background_t* restrict _background ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) ) {
        goto EXIT;
    }

    {
        l_returnValue = object_t$step( &( _background->object ), 0, 0 );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool background_t$render( const background_t* restrict _background,
                          const SDL_FRect* restrict _cameraRectangle,
                          bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        goto EXIT;
    }

    {
        l_returnValue = object_t$render( &( _background->object ),
                                         _cameraRectangle, _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
