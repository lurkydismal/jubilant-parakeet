#include "background_t.h"

#include "log.h"
#include "stdfunc.h"

background_t background_t$create( void ) {
    background_t l_returnValue = DEFAULT_BACKGROUND;

    {
        l_returnValue.object = object_t$create();

#if defined( DEBUG )

        l_returnValue.watches = createArray( watch_t* );

#endif
    }

    return ( l_returnValue );
}

bool background_t$destroy( background_t* restrict _background ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = object_t$destroy( &( _background->object ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Destroying background" );

            goto EXIT;
        }

        if ( LIKELY( _background->name ) ) {
            free( _background->name );
            _background->name = NULL;
        }

        if ( LIKELY( _background->folder ) ) {
            free( _background->folder );
            _background->folder = NULL;
        }

        if ( LIKELY( _background->extension ) ) {
            free( _background->extension );
            _background->extension = NULL;
        }

#if defined( DEBUG )

        FREE_ARRAY( _background->watches );

#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool background_t$reload$element( void* _context,
                                                      const char* _fileName,
                                                      uint32_t _eventId ) {
    background_t* l_background = ( background_t* )_context;

    log$transaction$query$format( ( logLevel_t )error, "CB %s %u %s", _fileName,
                                  _eventId, l_background->folder );

    return ( false );
}

bool background_t$load( background_t* restrict _background,
                        SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) || UNLIKELY( !_background->name ) ||
         UNLIKELY( !_background->folder ) ||
         UNLIKELY( !_background->extension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        log$transaction$query$format(
            ( logLevel_t )info, "Loading background: '%s'", _background->name );

        {
            const char* l_folder = _background->folder;

            {
                char* l_boxesGlob = NULL;

                // Boxes
                // folder/folder.boxes
                {
                    l_boxesGlob = duplicateString( l_folder );

                    concatBeforeAndAfterString( &l_boxesGlob, "/", ".boxes" );
                    concatBeforeAndAfterString( &l_boxesGlob, l_folder, NULL );
                }

                char* l_animationGlob = NULL;

                // Animation
                // folder/folder*.extension
                {
                    l_animationGlob = duplicateString( "*." );

                    concatBeforeAndAfterString( &l_animationGlob, l_folder,
                                                _background->extension );
                    concatBeforeAndAfterString( &l_animationGlob, "/", NULL );
                    concatBeforeAndAfterString( &l_animationGlob, l_folder,
                                                NULL );
                }

                l_returnValue = object_t$state$add$fromGlob(
                    &( _background->object ), _renderer, l_boxesGlob,
                    l_animationGlob, false, true );

                free( l_boxesGlob );
                free( l_animationGlob );
            }

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Adding background state from glob" );

                goto EXIT;
            }

            // Background always have only single state
            _background->object.currentState =
                arrayFirstElement( _background->object.states );

#if defined( DEBUG )
            // Watch
            {
                watch_t l_watch = watch_t$create();

                l_returnValue = watch_t$add$toPath( &l_watch, l_folder,
                                                    background_t$reload$element,
                                                    _background, true );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Adding background watch" );

                    goto EXIT;
                }

                insertIntoArray( &( _background->watches ), clone( &l_watch ) );
            }
#endif
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool background_t$unload( background_t* restrict _background ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = object_t$states$remove( &( _background->object ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Removing background states" );

            goto EXIT;
        }

        _background->object.currentState = NULL;

#if defined( DEBUG )
        // Watch
        {
            FOR_RANGE( arrayLength_t, 0, arrayLength( _background->watches ) ) {
                watch_t* l_watch = _background->watches[ _index ];

                l_returnValue = watch_t$destroy( l_watch );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Destroying background watch" );

                    goto EXIT;
                }

                free( l_watch );

                pluckArray( &( _background->watches ), l_watch );
            }
        }
#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool background_t$step( background_t* restrict _background ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_background ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = object_t$step( &( _background->object ), 0, 0 );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping background" );

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = object_t$render( &( _background->object ),
                                         _cameraRectangle, _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Rendering background" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
