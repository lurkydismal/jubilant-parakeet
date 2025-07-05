#include "character_t.h"

#include <cJSON.h>

#include "log.h"
#include "stdfunc.h"

#if defined( HOT_RELOAD )

#include "applicationState_t.h"

#endif

character_t character_t$create( void ) {
    character_t l_returnValue = DEFAULT_CHARACTER;

    {
        l_returnValue.movesObject = object_t$create();
        l_returnValue.moveNames = createArray( char* );
        l_returnValue.moves = createArray( move_t* );

#if defined( DEBUG )

        l_returnValue.watches = createArray( watch_t* );

#endif
    }

    return ( l_returnValue );
}

bool character_t$destroy( character_t* restrict _character ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_character ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = object_t$destroy( &( _character->movesObject ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Destroying character" );

            goto EXIT;
        }

        FREE_ARRAY( _character->moveNames );
        FREE_ARRAY( _character->moves );

        if ( LIKELY( _character->displayName ) ) {
            free( _character->displayName );
            _character->displayName = NULL;
        }

        if ( LIKELY( _character->name ) ) {
            free( _character->name );
            _character->name = NULL;
        }

        if ( LIKELY( _character->folder ) ) {
            free( _character->folder );
            _character->folder = NULL;
        }

        if ( LIKELY( _character->extension ) ) {
            free( _character->extension );
            _character->extension = NULL;
        }

#if defined( DEBUG )

        FREE_ARRAY( _character->watches );

#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO
static FORCE_INLINE bool load( character_t* restrict _character,
                               SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    {
        {
            char* l_boxesGlob = NULL;

            // Boxes
            // folder/folder.boxes
            {
                l_boxesGlob = duplicateString( _character->folder );

                concatBeforeAndAfterString( &l_boxesGlob, "/",
                                            "." BOXES_FILE_EXTENSION );
                concatBeforeAndAfterString( &l_boxesGlob, _character->folder,
                                            NULL );
            }

            char* l_animationGlob = NULL;

            // Animation
            // folder/folder*.extension
            {
                l_animationGlob = duplicateString( "_*." );

                concatBeforeAndAfterString( &l_animationGlob,
                                            _character->folder,
                                            _character->extension );
                concatBeforeAndAfterString( &l_animationGlob, "/", NULL );
                concatBeforeAndAfterString( &l_animationGlob,
                                            _character->folder, NULL );
            }

            l_returnValue = object_t$state$add$fromGlob(
                &( _character->movesObject ), _renderer, l_boxesGlob,
                l_animationGlob, "", false, true );

            free( l_boxesGlob );
            free( l_animationGlob );
        }

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Adding character state from glob" );

            goto EXIT;
        }

        // Background always have only single state
        _character->movesObject.currentState =
            arrayFirstElement( _character->movesObject.states );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO
static FORCE_INLINE bool character_t$reload$element(
    void* restrict _context,
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

        character_t* l_character = ( character_t* )_context;

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
                __builtin_strlen( l_character->folder );

            if ( __builtin_strncmp( _fileName, l_character->folder,
                                    l_folderLength ) == 0 ) {
                if ( ( __builtin_strcmp( l_fileExtension,
                                         l_character->extension ) == 0 ) &&
                     ( _fileName[ l_folderLength ] == '_' ) ) {
                    l_isAnimationFrame = true;

                } else if ( __builtin_strcmp( l_fileExtension,
                                              BOXES_FILE_EXTENSION ) == 0 ) {
                    char l_filePath[ PATH_MAX ] = { '\0' };

                    {
                        char* l_pointer = l_filePath;

                        l_pointer =
                            __builtin_stpcpy( l_pointer, l_character->folder );
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
            state_t* l_state =
                arrayFirstElement( l_character->movesObject.states );

            SDL_Renderer* l_renderer = l_state->renderer;

            l_returnValue =
                object_t$states$remove( &( l_character->movesObject ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing character state" );

                goto EXIT;
            }

            l_returnValue = load( l_character, l_renderer );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error,
                    "Loading character state %s from path: '%s'",
                    ( ( l_isAnimationFrame ) ? ( "animation" ) : ( "boxes" ) ),
                    _fileName );

                goto EXIT;
            }

            log$transaction$query$format(
                ( logLevel_t )debug,
                "Loaded character state %s from path: '%s'",
                ( ( l_isAnimationFrame ) ? ( "animation" ) : ( "boxes" ) ),
                _fileName );
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO
bool character_t$load( character_t* restrict _character,
                       SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_character ) || UNLIKELY( !_character->name ) ||
         UNLIKELY( !_character->folder ) ||
         UNLIKELY( !_character->extension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        log$transaction$query$format(
            ( logLevel_t )info, "Loading character: '%s'", _character->name );

        // Parse information
        {
            asset_t l_asset = asset_t$create();
            asset_t$load( &l_asset, "roa/info.json" );

            cJSON* l_root = cJSON_Parse(l_asset->data);

            {
                cJSON* l_name = cJSON_GetObjectItemCaseSensitive(l_root, "name");
                cJSON* l_startup = cJSON_GetObjectItemCaseSensitive(l_root, "startup");
                cJSON* l_active = cJSON_GetObjectItemCaseSensitive(l_root, "active");
                cJSON* l_recovery = cJSON_GetObjectItemCaseSensitive(l_root, "recovery");
                cJSON* l_cancelWindow = cJSON_GetObjectItemCaseSensitive(l_root, "cancelWindow");

                if (!cJSON_IsString(l_name) ||
                        !cJSON_IsNumber(l_startup) ||
                        !cJSON_IsNumber(l_active) ||
                        !cJSON_IsNumber(l_recovery) ||
                        !cJSON_IsArray(l_cancelWindow)) {
                    cJSON_Delete(l_root);
                    return false;
                }

                _outMove->name = strdup(l_name->valuestring);
                _outMove->startup = l_startup->valueint;
                _outMove->active = l_active->valueint;
                _outMove->recovery = l_recovery->valueint;

                cJSON* l_start = cJSON_GetArrayItem(l_cancelWindow, 0);
                cJSON* l_end = cJSON_GetArrayItem(l_cancelWindow, 1);

                if (!cJSON_IsNumber(l_start) || !cJSON_IsNumber(l_end)) {
                    cJSON_Delete(l_root);
                    return false;
                }

                _outMove->cancelStart = l_start->valueint;
                _outMove->cancelEnd = l_end->valueint;
            }

            cJSON_Delete(l_root);

            asset$unload( &l_asset );
            asset$destroy( &l_asset );
        }

        {
            l_returnValue = load( _character, _renderer );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading character" );

                goto EXIT;
            }

#if defined( DEBUG )
            // Watch
            {
                watch_t l_watch = watch_t$create();

                l_returnValue = watch_t$add$toPath(
                    &l_watch, _character->folder, character_t$reload,
                    _character, true );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Adding character watch" );

                    goto EXIT;
                }

                insertIntoArray( &( _character->watches ), clone( &l_watch ) );
            }
#endif
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO
bool character_t$unload( character_t* restrict _character ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_character ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = object_t$states$remove( &( _character->movesObject ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Removing character states" );

            goto EXIT;
        }

#if defined( DEBUG )
        // Watch
        {
            FOR_RANGE( arrayLength_t, 0, arrayLength( _character->watches ) ) {
                watch_t* l_watch = _character->watches[ _index ];

                l_returnValue = watch_t$destroy( l_watch );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Destroying character watch" );

                    goto EXIT;
                }

                free( l_watch );

                pluckArray( &( _character->watches ), l_watch );
            }
        }
#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
// Run scripts
bool character_t$step( character_t* restrict _character ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_character ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = object_t$step( &( _character->movesObject ), 0, 0 );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping character" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool character_t$render( const character_t* restrict _character,
                         const SDL_FRect* restrict _cameraRectangle,
                         bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_character ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = object_t$render( &( _character->movesObject ),
                                         _cameraRectangle, _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Rendering character" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#if defined( HOT_RELOAD )

bool hotReload$unload( void** restrict _state,
                       size_t* restrict _stateSize,
                       applicationState_t* restrict _applicationState ) {
    UNUSED( _state );
    UNUSED( _stateSize );

    if ( LIKELY( _applicationState->character ) &&
         LIKELY( _applicationState->character->watches ) ) {
        FOR_ARRAY( watch_t* const*, _applicationState->character->watches ) {
            watch_t* l_element = *_element;

            FOR_ARRAY( watchCallback_t*, l_element->watchCallbacks ) {
                *_element = NULL;
            }
        }
    }

    return ( true );
}

bool hotReload$load( void* restrict _state,
                     size_t _stateSize,
                     applicationState_t* restrict _applicationState ) {
    UNUSED( _state );
    UNUSED( _stateSize );

    if ( LIKELY( _applicationState->character ) &&
         LIKELY( _applicationState->character->watches ) ) {
        FOR_ARRAY( watch_t* const*, _applicationState->character->watches ) {
            watch_t* l_element = *_element;

            FOR_ARRAY( watchCallback_t*, l_element->watchCallbacks ) {
                *_element = character_t$reload;
            }
        }
    }

    return ( true );
}

#endif
