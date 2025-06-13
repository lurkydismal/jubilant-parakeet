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

            char* l_boxesGlob = NULL;

            // Boxes
            {
                l_boxesGlob = duplicateString( l_folder );

                concatBeforeAndAfterString( &l_boxesGlob, "/", ".boxes" );
                concatBeforeAndAfterString( &l_boxesGlob, l_folder, NULL );
            }

            char* l_animationGlob = NULL;

            // Animation
            {
                l_animationGlob = duplicateString( "*." );

                concatBeforeAndAfterString( &l_animationGlob, l_folder,
                                            _background->extension );
                concatBeforeAndAfterString( &l_animationGlob, "/", NULL );
                concatBeforeAndAfterString( &l_animationGlob, l_folder, NULL );
            }

            l_returnValue = object_t$state$add$fromGlob(
                &( _background->object ), _renderer, l_boxesGlob,
                l_animationGlob, false, true );

            free( l_boxesGlob );
            free( l_animationGlob );
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
