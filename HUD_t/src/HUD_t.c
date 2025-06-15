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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        FREE_ARRAY( _HUD->logos );
        FREE_ARRAY( _HUD->hpGauges );
        FREE_ARRAY( _HUD->hpBars );
        FREE_ARRAY( _HUD->names );
        FREE_ARRAY( _HUD->meterGauges );
        FREE_ARRAY( _HUD->meterBars );

        // Timer
        {
            l_returnValue = object_t$destroy( &( _HUD->timer ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying object\n" );

                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue = object_t$destroy( &( _HUD->timerBackground ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying object\n" );

                goto EXIT;
            }
        }
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool HUD_t$element$load$one(
    object_t* restrict _element,
    HUD_t* restrict _HUD,
    SDL_Renderer* _renderer,
    const char* restrict _subFolder ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_element ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_HUD ) || UNLIKELY( !_HUD->name ) ||
         UNLIKELY( !_HUD->folder ) || UNLIKELY( !_HUD->extension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_subFolder ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        // TODO: Improve
        {
            char* l_folder = duplicateString( "/" );

            concatBeforeAndAfterString( &l_folder, _HUD->folder, _subFolder );

            char* l_boxesGlbb = NULL;

            // Boxes
            {
                l_boxesGlbb = duplicateString( _HUD->folder );

                concatBeforeAndAfterString( &l_boxesGlbb, "/", ".boxes" );
                concatBeforeAndAfterString( &l_boxesGlbb, l_folder, NULL );
            }

            char* l_animationGlob = NULL;

            // Animation
            {
                l_animationGlob = duplicateString( "*." );

                concatBeforeAndAfterString( &l_animationGlob, _HUD->folder,
                                            _HUD->extension );
                concatBeforeAndAfterString( &l_animationGlob, "/", NULL );
                concatBeforeAndAfterString( &l_animationGlob, l_folder, NULL );
            }

            l_returnValue =
                object_t$state$add$fromGlob( _element, _renderer, l_boxesGlbb,
                                             l_animationGlob, false, true );

            free( l_boxesGlbb );
            free( l_animationGlob );

            free( l_folder );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Adding object state from glob\n" );

                goto EXIT;
            }
        }

        // Background always have only single state
        _element->currentState = arrayFirstElement( _element->states );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$load( HUD_t* restrict _HUD,
                 SDL_Renderer* _renderer,
                 const size_t _amount ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) || UNLIKELY( !_HUD->name ) ||
         UNLIKELY( !_HUD->folder ) || UNLIKELY( !_HUD->extension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_amount ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )info, "Loading HUD: '%s'\n",
                                      _HUD->name );

#define TRY_LOAD_OR_EXIT( _field )                                            \
    do {                                                                      \
        FOR_ARRAY( object_t* const*, _HUD->_field ) {                         \
            object_t l_element = object_t$create();                           \
            l_returnValue = HUD_t$element$load$one( &l_element, _HUD,         \
                                                    _renderer, #_field "/" ); \
            if ( UNLIKELY( !l_returnValue ) ) {                               \
                log$transaction$query( ( logLevel_t )error,                   \
                                       "Loading HUD " #_field "\n" );         \
                goto EXIT;                                                    \
            }                                                                 \
            insertIntoArray( &( _HUD->_field ), clone( &l_element ) );        \
        }                                                                     \
    } while ( 0 )

        // TODO: Fix
        // TRY_LOAD_OR_EXIT( logos );

#undef TRY_LOAD_OR_EXIT

        // Timer
        {
            l_returnValue = HUD_t$element$load$one( &( _HUD->timer ), _HUD,
                                                    _renderer, "timer/" );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading HUD timer\n" );

                goto EXIT;
            }
        }

        // Timer background
#if 0
        {
            l_returnValue =
                HUD_t$element$load$one( &( _HUD->timerBackground ), _HUD,
                                        _renderer, "timerBackground/" );

            if ( UNLIKELY( !l_returnValue ) ) {
        log$transaction$query( ( logLevel_t )error, "Loading HUD timer background\n" );

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
#define REMOVE_STATES_IN_OBJECTS_AND_FREE_OR_EXIT( _objects )        \
    do {                                                             \
        if ( UNLIKELY( !( _objects ) ) ) {                           \
            l_returnValue = false;                                   \
            log$transaction$query( ( logLevel_t )error,              \
                                   "Invalid argument\n" );           \
            goto EXIT;                                               \
        }                                                            \
        FOR_ARRAY( object_t* const*, ( _objects ) ) {                \
            l_returnValue = object_t$states$remove( *_element );     \
            if ( UNLIKELY( !l_returnValue ) ) {                      \
                log$transaction$query( ( logLevel_t )error,          \
                                       "Removing object states\n" ); \
                goto EXIT;                                           \
            }                                                        \
            l_returnValue = object_t$destroy( *_element );           \
            if ( UNLIKELY( !l_returnValue ) ) {                      \
                log$transaction$query( ( logLevel_t )error,          \
                                       "Destroying object\n" );      \
                goto EXIT;                                           \
            }                                                        \
            FREE_ARRAY_ELEMENTS( _objects );                         \
        }                                                            \
    } while ( 0 )

        REMOVE_STATES_IN_OBJECTS_AND_FREE_OR_EXIT( _HUD->logos );
        REMOVE_STATES_IN_OBJECTS_AND_FREE_OR_EXIT( _HUD->hpGauges );
        REMOVE_STATES_IN_OBJECTS_AND_FREE_OR_EXIT( _HUD->hpBars );
        REMOVE_STATES_IN_OBJECTS_AND_FREE_OR_EXIT( _HUD->names );
        REMOVE_STATES_IN_OBJECTS_AND_FREE_OR_EXIT( _HUD->meterGauges );
        REMOVE_STATES_IN_OBJECTS_AND_FREE_OR_EXIT( _HUD->meterBars );

#undef REMOVE_STATES_IN_OBJECTS_AND_FREE_OR_EXIT

        // Timer
        {
            l_returnValue = object_t$states$remove( &( _HUD->timer ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing object states\n" );

                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue =
                object_t$states$remove( &( _HUD->timerBackground ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing object states\n" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Accept app state
bool HUD_t$step( HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
#define STEP_OBJECTS_OR_EXIT( _objects )                      \
    do {                                                      \
        FOR_ARRAY( object_t* const*, ( _objects ) ) {         \
            l_returnValue = object_t$step( *_element, 0, 0 ); \
            if ( UNLIKELY( !l_returnValue ) ) {               \
                log$transaction$query( ( logLevel_t )error,   \
                                       "Stepping object\n" ); \
                goto EXIT;                                    \
            }                                                 \
        }                                                     \
    } while ( 0 )

        STEP_OBJECTS_OR_EXIT( _HUD->logos );
        STEP_OBJECTS_OR_EXIT( _HUD->hpGauges );
        STEP_OBJECTS_OR_EXIT( _HUD->hpBars );
        STEP_OBJECTS_OR_EXIT( _HUD->names );
        STEP_OBJECTS_OR_EXIT( _HUD->meterGauges );
        STEP_OBJECTS_OR_EXIT( _HUD->meterBars );

#undef STEP_OBJECTS_OR_EXIT

        // Timer
        {
            l_returnValue = object_t$step( &( _HUD->timer ), 0, 0 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Stepping object\n" );

                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue = object_t$step( &( _HUD->timerBackground ), 0, 0 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Stepping object\n" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Accept app state
bool HUD_t$render( const HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        const SDL_FRect l_cameraRectangle = { .x = 0, .y = 0, .w = 0, .h = 0 };
        const bool l_doDrawBoxes = false;

#define RENDER_OBJECTS_OR_EXIT( _objects )                                  \
    do {                                                                    \
        FOR_ARRAY( object_t* const*, ( _objects ) ) {                       \
            l_returnValue = object_t$render( *_element, &l_cameraRectangle, \
                                             l_doDrawBoxes );               \
            if ( UNLIKELY( !l_returnValue ) ) {                             \
                log$transaction$query( ( logLevel_t )error,                 \
                                       "Rendering object\n" );              \
                goto EXIT;                                                  \
            }                                                               \
        }                                                                   \
    } while ( 0 )

        RENDER_OBJECTS_OR_EXIT( _HUD->logos );
        RENDER_OBJECTS_OR_EXIT( _HUD->hpGauges );
        RENDER_OBJECTS_OR_EXIT( _HUD->hpBars );
        RENDER_OBJECTS_OR_EXIT( _HUD->names );
        RENDER_OBJECTS_OR_EXIT( _HUD->meterGauges );
        RENDER_OBJECTS_OR_EXIT( _HUD->meterBars );

#undef RENDER_OBJECTS_OR_EXIT

        // Timer
        {
            l_returnValue = object_t$render(
                &( _HUD->timer ), &l_cameraRectangle, l_doDrawBoxes );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Rendering object\n" );

                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue = object_t$render(
                &( _HUD->timer ), &l_cameraRectangle, l_doDrawBoxes );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Rendering object\n" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
