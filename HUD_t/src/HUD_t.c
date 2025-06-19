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

        l_returnValue.timer = object_t$create();
        l_returnValue.timerBackground = object_t$create();
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

        // Timer
        {
            l_returnValue = object_t$destroy( &( _HUD->timer ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying object" );

                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue = object_t$destroy( &( _HUD->timerBackground ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying object" );

                goto EXIT;
            }
        }

        _HUD->name = NULL;
        _HUD->folder = NULL;
        _HUD->extension = NULL;
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

                concatBeforeAndAfterString( &l_boxesGlbb, "/", ".boxes" );
                concatBeforeAndAfterString( &l_boxesGlbb, l_folder, NULL );
            }

            char* l_animationGlob = NULL;

            // Animation
            // folder/folder*.extension
            {
                l_animationGlob = duplicateString( "*." );

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

#if 0
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
#endif

        // Timer background
#if 1
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
                                       "Destroying object" );                 \
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

        // Timer
        {
            l_returnValue = object_t$states$remove( &( _HUD->timer ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing timer states" );

                goto EXIT;
            }
        }

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

        free( _HUD->name );
        _HUD->name = NULL;

        free( _HUD->folder );
        _HUD->folder = NULL;

        free( _HUD->extension );
        _HUD->extension = NULL;

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

#if 0
        // Timer
        {
            l_returnValue = object_t$step( &( _HUD->timer ), 0, 0 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Stepping timer" );

                goto EXIT;
            }
        }
#endif

#if 1
        // Timer background
        {
            l_returnValue = object_t$step( &( _HUD->timerBackground ), 0, 0 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Stepping timer background" );

                goto EXIT;
            }
        }
#endif

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

#if 0
        // Timer
        {
            l_returnValue = object_t$render(
                &( _HUD->timer ), &l_cameraRectangle, l_doDrawBoxes );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Rendering timer" );

                goto EXIT;
            }
        }
#endif

#if 1
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
#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
