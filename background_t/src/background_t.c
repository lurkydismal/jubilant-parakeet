#include "background_t.h"

#include "log.h"
#include "stdfunc.h"

#if defined( HOT_RELOAD )

#include "applicationState_t.h"

#endif

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

static FORCE_INLINE bool load( background_t* restrict _background,
                               SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    {
        {
            char* l_boxesGlob = NULL;

            // Boxes
            // folder/folder.boxes
            {
                l_boxesGlob = duplicateString( _background->folder );

                concatBeforeAndAfterString( &l_boxesGlob, "/",
                                            "." BOXES_FILE_EXTENSION );
                concatBeforeAndAfterString( &l_boxesGlob, _background->folder,
                                            NULL );
            }

            char* l_animationGlob = NULL;

            // Animation
            // folder/folder*.extension
            {
                l_animationGlob = duplicateString( "_*." );

                concatBeforeAndAfterString( &l_animationGlob,
                                            _background->folder,
                                            _background->extension );
                concatBeforeAndAfterString( &l_animationGlob, "/", NULL );
                concatBeforeAndAfterString( &l_animationGlob,
                                            _background->folder, NULL );
            }

            char* l_stateName = ( char* )__builtin_alloca( 1 * sizeof( char ) );
            l_stateName[ 0 ] = '\0';

            l_returnValue = object_t$state$add$fromGlob(
                &( _background->object ), _renderer, l_boxesGlob,
                l_animationGlob, l_stateName, false, true );

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

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool background_t$reload( void* restrict _context,
                                              const char* restrict _fileName,
                                              size_t _eventMask,
                                              uint32_t _cookie ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_context ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_fileName ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#if defined( LOG_WATCH )

        log$transaction$query$format(
            ( logLevel_t )info,
            "Background watch: file [ '%s' : '%#zx' : '%u' ]", _fileName,
            _eventMask, _cookie );

#else

        UNUSED( _eventMask );
        UNUSED( _cookie );

#endif

        background_t* l_background = ( background_t* )_context;

        bool l_isAnimationFrame = false;
        bool l_isBoxes = false;

        {
            const ssize_t l_fileExtensionStartIndex =
                findLastSymbolInString( _fileName, '.' );

            l_returnValue = ( l_fileExtensionStartIndex != -1 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )warn,
                                       "No file extension" );

                l_returnValue = true;

                goto EXIT;
            }

            const char* l_fileExtension =
                ( _fileName + l_fileExtensionStartIndex + 1 );
            const size_t l_folderLength =
                __builtin_strlen( l_background->folder );

            if ( __builtin_strncmp( _fileName, l_background->folder,
                                    l_folderLength ) == 0 ) {
                if ( ( __builtin_strcmp( l_fileExtension,
                                         l_background->extension ) == 0 ) &&
                     ( _fileName[ l_folderLength ] == '_' ) ) {
                    l_isAnimationFrame = true;

                } else if ( __builtin_strcmp( l_fileExtension,
                                              BOXES_FILE_EXTENSION ) == 0 ) {
                    char l_filePath[ PATH_MAX ] = { '\0' };

                    {
                        char* l_pointer = l_filePath;

                        l_pointer =
                            __builtin_stpcpy( l_pointer, l_background->folder );
                        l_pointer = __builtin_stpcpy( l_pointer, "/" );
                        __builtin_stpcpy( l_pointer, _fileName );
                    }

                    if ( doesPathExist( l_filePath ) ) {
                        l_isBoxes = true;
                    }
                }
            }
        }

        if ( l_isAnimationFrame || l_isBoxes ) {
            state_t* l_state = arrayFirstElement( l_background->object.states );

            SDL_Renderer* l_renderer = l_state->renderer;

            l_returnValue = object_t$states$remove( &( l_background->object ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing background state" );

                goto EXIT;
            }

            l_returnValue = load( l_background, l_renderer );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error,
                    "Loading background state %s from path: '%s'",
                    ( ( l_isAnimationFrame ) ? ( "animation" ) : ( "boxes" ) ),
                    _fileName );

                goto EXIT;
            }

            log$transaction$query$format(
                ( logLevel_t )debug,
                "Loaded background state %s from path: '%s'",
                ( ( l_isAnimationFrame ) ? ( "animation" ) : ( "boxes" ) ),
                _fileName );
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
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
            l_returnValue = load( _background, _renderer );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading background" );

                goto EXIT;
            }

#if defined( DEBUG )
            // Watch
            {
                watch_t l_watch = watch_t$create();

                l_returnValue = watch_t$add$toPath(
                    &l_watch, _background->folder, background_t$reload,
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

#if defined( HOT_RELOAD )

EXPORT bool hotReload$unload( void** restrict _state,
                              size_t* restrict _stateSize,
                              applicationState_t* restrict _applicationState ) {
    UNUSED( _state );
    UNUSED( _stateSize );

    if ( LIKELY( _applicationState->background ) &&
         LIKELY( _applicationState->background->watches ) ) {
        FOR_ARRAY( watch_t* const*, _applicationState->background->watches ) {
            watch_t* l_element = *_element;

            FOR_ARRAY( watchCallback_t*, l_element->watchCallbacks ) {
                *_element = NULL;
            }
        }
    }

    return ( true );
}

EXPORT bool hotReload$load( void* restrict _state,
                            size_t _stateSize,
                            applicationState_t* restrict _applicationState ) {
    UNUSED( _state );
    UNUSED( _stateSize );

    if ( LIKELY( _applicationState->background ) &&
         LIKELY( _applicationState->background->watches ) ) {
        FOR_ARRAY( watch_t* const*, _applicationState->background->watches ) {
            watch_t* l_element = *_element;

            FOR_ARRAY( watchCallback_t*, l_element->watchCallbacks ) {
                *_element = background_t$reload;
            }
        }
    }

    return ( true );
}

#endif
