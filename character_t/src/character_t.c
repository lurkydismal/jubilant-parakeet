#include "character_t.h"

#include <cJSON.h>

#if 0
#include "asset_t.h"
#endif
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
#if 0
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

            char* l_stateName = ( char* )__builtin_alloca( 1 * sizeof( char ) );
            l_stateName[ 0 ] = '\0';

            l_returnValue = object_t$state$add$fromGlob(
                &( _character->movesObject ), _renderer, l_boxesGlob,
                l_animationGlob, l_stateName, false, true );

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
#endif

// TODO
#if 0
static FORCE_INLINE bool character_t$reload( void* restrict _context,
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
            const size_t l_fileExtensionStartIndex =
                findLastSymbolInString( _fileName, '.' );

            l_returnValue = ( l_fileExtensionStartIndex != SIZE_MAX );

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
#endif

#if 0
move_t* parseMoves(const cJSON* root) {
    for (size_t i = 0; i < count; ++i) {
        // — velocity (float[2])

        // — cancelWindows (array of [start,end])
        cJSON* cw = cJSON_GetObjectItemCaseSensitive(l_move, "cancelWindows");
        if (cJSON_IsArray(cw)) {
            cJSON* pair = NULL;
            cJSON_ArrayForEach(pair, cw) {
                if (cJSON_IsArray(pair) && cJSON_GetArraySize(pair) == 2) {
                    size_t* range = malloc(2 * sizeof(size_t));
                    range[0] = (size_t)cJSON_GetArrayItem(pair,0)->valueint;
                    range[1] = (size_t)cJSON_GetArrayItem(pair,1)->valueint;
                    insertIntoArray(&m->cancelWindows, range);
                }
            }
        }

        // — startup / active / recovery
        JSON_SET_AS(l_move, "startup",  m->startup);
        JSON_SET_AS(l_move, "active",   m->active);
        JSON_SET_AS(l_move, "recovery", m->recovery);

        // — cancelInto (array of strings → IDs)
        cJSON* ci = cJSON_GetObjectItemCaseSensitive(l_move, "cancelInto");
        if (cJSON_IsArray(ci)) {
            cJSON* entry = NULL;
            cJSON_ArrayForEach(entry, ci) {
                if (cJSON_IsString(entry)) {
                    size_t id = convertTypeStringToID(entry->valuestring);
                    insertIntoArray(&m->cancelInto, id);
                }
            }
        }
    }
    return moves;
}
#endif

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

#if 0
    {
        log$transaction$query$format(
            ( logLevel_t )info, "Loading character: '%s'", _character->name );

        // Parse information
        {
#define JSON_GET_AS( _root, _field, _type )                                   \
    ( {                                                                       \
        cJSON* _item =                                                        \
            cJSON_GetObjectItemCaseSensitive( ( _root ), ( _field ) );        \
        _Generic( ( ( _type )0 ),                                             \
            int: ( ( _item && cJSON_IsNumber( _item ) ) ? ( _item->valueint ) \
                                                        : ( 0 ) ),            \
            double: ( ( _item && cJSON_IsNumber( _item ) )                    \
                          ? ( _item->valuedouble )                            \
                          : ( 0.0 ) ),                                        \
            float: ( ( _item && cJSON_IsNumber( _item ) )                     \
                         ? ( _item->valuedouble )                             \
                         : ( 0.0 ) ),                                         \
            char*: ( ( _item && cJSON_IsString( _item ) )                     \
                         ? ( _item->valuestring )                             \
                         : ( NULL ) ) );                                      \
    } )

#define JSON_SET_AS( _root, _field, _variable )                            \
    do {                                                                   \
        ( _variable ) = JSON_GET_AS( _root, _field, typeof( _variable ) ); \
    } while ( 0 )

            asset_t l_asset = asset_t$create();
            asset_t$load$fromPath( &l_asset, "roa/roa.json" );

            cJSON* l_root = cJSON_Parse((char*)(l_asset.data));

            {
                // Character characteristics
                {
                    JSON_SET_AS(l_root, "displayName", _character->displayName);
                    JSON_SET_AS( l_root, "healthPoints", _character->healthPointsMax );
                    JSON_SET_AS( l_root, "guardPoints", _character->guardPointsMax );
                    JSON_SET_AS( l_root, "meterPoints", _character->meterPointsMax );
                    JSON_SET_AS( l_root, "walkSpeed", _character->walkSpeed );
                    JSON_SET_AS( l_root, "jumpHeight", _character->jumpHeight );
                }

                // Moves
                {
                    move_t* moves = parseMoves( l_root );

                    cJSON* l_moves = cJSON_GetObjectItemCaseSensitive(l_root, "moves");

                    if (UNLIKELY(!cJSON_IsArray(l_moves))) {
                        goto EXIT;
                    }

                    FOR_RANGE( int, 0, cJSON_GetArraySize(l_moves) ) {
                        cJSON* l_move = cJSON_GetArrayItem(l_moves, i);

                        if (UNLIKELY(!cJSON_IsObject(l_move))) {
                            continue;
                        }

                        JSON_SET_AS(l_move, "type",  move->type);
                        move->input = parseInputString(JSON_GET_AS(l_move, "input", char*));

                        cJSON* vel = cJSON_GetObjectItemCaseSensitive(l_move, "velocity");

                        if (cJSON_IsArray(vel) && cJSON_GetArraySize(vel) == 2) {
                            m->velocity = malloc(2 * sizeof(float));
                            m->velocity[0] = (float)cJSON_GetArrayItem(vel, 0)->valuedouble;
                            m->velocity[1] = (float)cJSON_GetArrayItem(vel, 1)->valuedouble;
                        }
                    }
                }
            }

            cJSON_Delete(l_root);

            asset_t$unload( &l_asset );
            asset_t$destroy( &l_asset );

#undef JSON_SET_AS
#undef JSON_GET_AS
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
#endif

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

                pluckArrayByValue( &( _character->watches ), l_watch );
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

EXPORT bool hotReload$unload( void** restrict _state,
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

EXPORT bool hotReload$load( void* restrict _state,
                            size_t _stateSize,
                            applicationState_t* restrict _applicationState ) {
    UNUSED( _state );
    UNUSED( _stateSize );

    if ( LIKELY( _applicationState->character ) &&
         LIKELY( _applicationState->character->watches ) ) {
        FOR_ARRAY( watch_t* const*, _applicationState->character->watches ) {
            watch_t* l_element = *_element;

            FOR_ARRAY( watchCallback_t*, l_element->watchCallbacks ) {
#if 0
                *_element = character_t$reload;
#endif
            }
        }
    }

    return ( true );
}

#endif
