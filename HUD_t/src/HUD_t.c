#include "HUD_t.h"

#include "log.h"
#include "stdfunc.h"

HUD_t HUD_t$create( void ) {
    HUD_t l_returnValue = DEFAULT_HUD;

    {
        l_returnValue.logos = createArray( object_t* );
        l_returnValue.hpGauges = createArray( object_t* );
        l_returnValue.hpBars = createArray( object_t* );
        l_returnValue.names = createArray( object_t* );
        l_returnValue.meterGauges = createArray( object_t* );
        l_returnValue.meterBars = createArray( object_t* );

        l_returnValue.timerBackground = object_t$create();
        l_returnValue.timer = object_t$create();

#if defined( DEBUG )

        l_returnValue.watches = createArray( watch_t* );

#endif
    }

    return ( l_returnValue );
}

bool HUD_t$destroy( HUD_t* _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        FREE_ARRAY( _HUD->logos );
        _HUD->logos = NULL;

        FREE_ARRAY( _HUD->hpGauges );
        _HUD->hpGauges = NULL;

        FREE_ARRAY( _HUD->hpBars );
        _HUD->hpBars = NULL;

        FREE_ARRAY( _HUD->names );
        _HUD->names = NULL;

        FREE_ARRAY( _HUD->meterGauges );
        _HUD->meterGauges = NULL;

        FREE_ARRAY( _HUD->meterBars );
        _HUD->meterBars = NULL;

        // Timer background
        {
            l_returnValue = object_t$destroy( &( _HUD->timerBackground ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying timer background" );

                goto EXIT;
            }
        }

        // Timer
        {
            l_returnValue = object_t$destroy( &( _HUD->timer ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying timer" );

                goto EXIT;
            }
        }

        if ( LIKELY( _HUD->name ) ) {
            free( _HUD->name );
            _HUD->name = NULL;
        }

        if ( LIKELY( _HUD->folder ) ) {
            free( _HUD->folder );
            _HUD->folder = NULL;
        }

        if ( LIKELY( _HUD->extension ) ) {
            free( _HUD->extension );
            _HUD->extension = NULL;
        }

        _HUD->playerAmount = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool HUD_t$element$load$one(
    object_t* restrict _element,
    HUD_t* restrict _HUD,
    SDL_Renderer* _renderer,
    const char* restrict _fieldName ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_element ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_HUD ) || UNLIKELY( !_HUD->name ) ||
         UNLIKELY( !_HUD->folder ) || UNLIKELY( !_HUD->extension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_fieldName ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        {
            char* l_folder = _HUD->folder;

            char* l_boxesGlbb = NULL;

            // Boxes
            // folder/folder.boxes
            {
                l_boxesGlbb = duplicateString( _fieldName );

                concatBeforeAndAfterString( &l_boxesGlbb, "/",
                                            "." BOXES_FILE_EXTENSION );
                concatBeforeAndAfterString( &l_boxesGlbb, l_folder, NULL );
            }

            char* l_animationGlob = NULL;

            // Animation
            // folder/folder*.extension
            {
                l_animationGlob = duplicateString( "_*." );

                concatBeforeAndAfterString( &l_animationGlob, _fieldName,
                                            _HUD->extension );
                concatBeforeAndAfterString( &l_animationGlob, "/", NULL );
                concatBeforeAndAfterString( &l_animationGlob, l_folder, NULL );
            }

            l_returnValue =
                object_t$state$add$fromGlob( _element, _renderer, l_boxesGlbb,
                                             l_animationGlob, false, true );

            free( l_boxesGlbb );
            free( l_animationGlob );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Adding HUD element state from glob" );

                goto EXIT;
            }
        }

        // Default to the first state
        _element->currentState = arrayFirstElement( _element->states );

        // World coordinates is the same as camera coordinates for HUD element
        // Define world/ camera coordinates
        {
            const SDL_FRect* l_boxesKeyFramesFirstElement =
                arrayFirstElement( _element->currentState->boxes.keyFrames );

            _element->worldX = l_boxesKeyFramesFirstElement->x;
            _element->worldXMin = l_boxesKeyFramesFirstElement->x;
            _element->worldXMax = l_boxesKeyFramesFirstElement->x;

            _element->worldY = l_boxesKeyFramesFirstElement->y;
            _element->worldYMin = l_boxesKeyFramesFirstElement->y;
            _element->worldYMax = l_boxesKeyFramesFirstElement->y;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool HUD_t$reload$element( void* _context,
                                               const char* _fileName,
                                               size_t _eventsMask,
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
        log$transaction$query$format( ( logLevel_t )info,
                                      "Background watch: file [ '%s' : '%u' ]",
                                      _fileName, _cookie );

        HUD_t* l_HUD = ( HUD_t* )_context;

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

            if ( ( __builtin_strcmp( l_fileExtension, l_HUD->extension ) ==
                   0 ) &&
                 // File name
                 ( ( __builtin_strncmp( _fileName, l_HUD->folder,
                                        __builtin_strlen( l_HUD->folder ) ) ==
                     0 ) &&
                   ( _fileName[ __builtin_strlen( l_HUD->folder ) ] ==
                     '_' ) ) ) {
                l_isAnimationFrame = true;

            } else {
                char* l_boxesFileName = duplicateString( l_HUD->folder );

                concatBeforeAndAfterString( &l_boxesFileName, NULL,
                                            "." BOXES_FILE_EXTENSION );

                if ( __builtin_strcmp( _fileName, l_boxesFileName ) == 0 ) {
                    l_isBoxes = true;
                }

                free( l_boxesFileName );
            }
        }

        if ( l_isAnimationFrame || l_isBoxes ) {
            object_t* l_object = NULL;

            // TODO: Add other
#define MACRO( _field )                                                       \
    do {                                                                      \
        const size_t l_##_field##Length = __builtin_strlen( #_field );        \
        if ( ( __builtin_strncmp( _fileName, #_field, l_##_field##Length ) == \
               0 ) &&                                                         \
             ( _fileName[ l_##_field##Length ] == '_' ) ) {                   \
            l_object = &( l_HUD->_field );                                    \
        }                                                                     \
    } while ( 0 )

            MACRO( timer );
            MACRO( timerBackground );

#undef MACRO

            if ( UNLIKELY( !l_object ) ) {
                // TODO: Improve
                log$transaction$query( ( logLevel_t )error,
                                       "Converting X to X" );

                goto EXIT;
            }

            state_t* l_state = arrayFirstElement( l_object->states );

            char** l_paths = createArray( char* );

            char* l_path = duplicateString( "/" );

            concatBeforeAndAfterString( &l_path, l_HUD->folder, _fileName );

            insertIntoArray( &l_paths, l_path );

            if ( l_isAnimationFrame ) {
                l_returnValue = animation_t$load$fromPaths(
                    &( l_state->animation ), l_state->renderer, l_paths );

            } else if ( l_isBoxes ) {
                l_returnValue =
                    boxes_t$load$fromPaths( &( l_state->boxes ), l_paths );
            }

            FREE_ARRAY_ELEMENTS( l_paths );
            FREE_ARRAY( l_paths );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Loading HUD state %s from path: '%s'",
                    ( ( l_isAnimationFrame ) ? ( "animation" ) : ( "boxes" ) ),
                    _fileName );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$load( HUD_t* restrict _HUD, SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) || UNLIKELY( !_HUD->name ) ||
         UNLIKELY( !_HUD->folder ) || UNLIKELY( !_HUD->extension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_HUD->playerAmount ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )info, "Loading HUD: '%s'",
                                      _HUD->name );

#define TRY_LOAD_OR_EXIT( _field )                                        \
    do {                                                                  \
        FOR_RANGE( size_t, 0, _HUD->playerAmount ) {                      \
            object_t l_element = object_t$create();                       \
            l_returnValue = HUD_t$element$load$one( &l_element, _HUD,     \
                                                    _renderer, #_field ); \
            if ( UNLIKELY( !l_returnValue ) ) {                           \
                log$transaction$query( ( logLevel_t )error,               \
                                       "Loading HUD " #_field );          \
                goto EXIT;                                                \
            }                                                             \
            insertIntoArray( &( _HUD->_field ), clone( &l_element ) );    \
        }                                                                 \
    } while ( 0 )

        // TODO: Fix
        // TRY_LOAD_OR_EXIT( logos );

#undef TRY_LOAD_OR_EXIT

        // Timer background
        {
            l_returnValue =
                HUD_t$element$load$one( &( _HUD->timerBackground ), _HUD,
                                        _renderer, "timerBackground" );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading HUD timer background" );

                goto EXIT;
            }
        }

        // Timer
        {
            l_returnValue = HUD_t$element$load$one( &( _HUD->timer ), _HUD,
                                                    _renderer, "timer" );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading HUD timer" );

                goto EXIT;
            }
        }

#if defined( DEBUG )
        // Watch
        {
            watch_t l_watch = watch_t$create();

            l_returnValue = watch_t$add$toPath(
                &l_watch, _HUD->folder, HUD_t$reload$element, _HUD, true );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Adding HUD watch" );

                goto EXIT;
            }

            insertIntoArray( &( _HUD->watches ), clone( &l_watch ) );
        }
#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$unload( HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define REMOVE_STATES_AND_FREE_OR_EXIT( _field )                              \
    do {                                                                      \
        if ( UNLIKELY( !( _HUD->_field ) ) ) {                                \
            l_returnValue = false;                                            \
            log$transaction$query( ( logLevel_t )error, "Invalid argument" ); \
            goto EXIT;                                                        \
        }                                                                     \
        FOR_ARRAY( object_t* const*, ( _HUD->_field ) ) {                     \
            l_returnValue = object_t$states$remove( *_element );              \
            if ( UNLIKELY( !l_returnValue ) ) {                               \
                log$transaction$query( ( logLevel_t )error,                   \
                                       "Removing HUD " #_field " states" );   \
                goto EXIT;                                                    \
            }                                                                 \
            l_returnValue = object_t$destroy( *_element );                    \
            if ( UNLIKELY( !l_returnValue ) ) {                               \
                log$transaction$query( ( logLevel_t )error,                   \
                                       "Destroying HUD " #_field );           \
                goto EXIT;                                                    \
            }                                                                 \
        }                                                                     \
        FREE_ARRAY_ELEMENTS( _HUD->_field );                                  \
    } while ( 0 )

        REMOVE_STATES_AND_FREE_OR_EXIT( logos );
        REMOVE_STATES_AND_FREE_OR_EXIT( hpGauges );
        REMOVE_STATES_AND_FREE_OR_EXIT( hpBars );
        REMOVE_STATES_AND_FREE_OR_EXIT( names );
        REMOVE_STATES_AND_FREE_OR_EXIT( meterGauges );
        REMOVE_STATES_AND_FREE_OR_EXIT( meterBars );

#undef REMOVE_STATES_AND_FREE_OR_EXIT

        // Timer background
        {
            l_returnValue =
                object_t$states$remove( &( _HUD->timerBackground ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing timer background states" );

                goto EXIT;
            }
        }

        // Timer
        {
            l_returnValue = object_t$states$remove( &( _HUD->timer ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing timer states" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$step( HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define STEP_OBJECTS_OR_EXIT( _field )                        \
    do {                                                      \
        FOR_ARRAY( object_t* const*, _HUD->_field ) {         \
            l_returnValue = object_t$step( *_element, 0, 0 ); \
            if ( UNLIKELY( !l_returnValue ) ) {               \
                log$transaction$query( ( logLevel_t )error,   \
                                       "Stepping " #_field ); \
                goto EXIT;                                    \
            }                                                 \
        }                                                     \
    } while ( 0 )

        STEP_OBJECTS_OR_EXIT( logos );
        STEP_OBJECTS_OR_EXIT( hpGauges );
        STEP_OBJECTS_OR_EXIT( hpBars );
        STEP_OBJECTS_OR_EXIT( names );
        STEP_OBJECTS_OR_EXIT( meterGauges );
        STEP_OBJECTS_OR_EXIT( meterBars );

#undef STEP_OBJECTS_OR_EXIT

        // Timer background
        {
            l_returnValue = object_t$step( &( _HUD->timerBackground ), 0, 0 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Stepping timer background" );

                goto EXIT;
            }
        }

        // Timer
        {
            l_returnValue = object_t$step( &( _HUD->timer ), 0, 0 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Stepping timer" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$render( const HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const SDL_FRect l_cameraRectangle = { .x = 0, .y = 0, .w = 0, .h = 0 };
        const bool l_doDrawBoxes = false;

#define RENDER_OBJECTS_OR_EXIT( _field )                                    \
    do {                                                                    \
        FOR_ARRAY( object_t* const*, ( _HUD->_field ) ) {                   \
            l_returnValue = object_t$render( *_element, &l_cameraRectangle, \
                                             l_doDrawBoxes );               \
            if ( UNLIKELY( !l_returnValue ) ) {                             \
                log$transaction$query( ( logLevel_t )error,                 \
                                       "Rendering " #_field );              \
                goto EXIT;                                                  \
            }                                                               \
        }                                                                   \
    } while ( 0 )

        RENDER_OBJECTS_OR_EXIT( logos );
        RENDER_OBJECTS_OR_EXIT( hpGauges );
        RENDER_OBJECTS_OR_EXIT( hpBars );
        RENDER_OBJECTS_OR_EXIT( names );
        RENDER_OBJECTS_OR_EXIT( meterGauges );
        RENDER_OBJECTS_OR_EXIT( meterBars );

#undef RENDER_OBJECTS_OR_EXIT

        // Timer background
        {
            l_returnValue = object_t$render(
                &( _HUD->timerBackground ), &l_cameraRectangle, l_doDrawBoxes );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Rendering timer background" );

                goto EXIT;
            }
        }

        // Timer
        {
            l_returnValue = object_t$render(
                &( _HUD->timer ), &l_cameraRectangle, l_doDrawBoxes );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Rendering timer" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
