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
        goto EXIT;
    }

    {
#define DESTROY_AND_FREE_OBJECTS( _objects )               \
    do {                                                   \
        if ( UNLIKELY( !( _objects ) ) ) {                 \
            l_returnValue = false;                         \
            goto EXIT;                                     \
        }                                                  \
        FOR_ARRAY( object_t* const*, ( _objects ) ) {      \
            l_returnValue = object_t$destroy( *_element ); \
            if ( UNLIKELY( !l_returnValue ) ) {            \
                goto EXIT;                                 \
            }                                              \
        }                                                  \
        FREE_ARRAY_ELEMENTS( _objects );                   \
        FREE_ARRAY( _objects );                            \
    } while ( 0 )

        DESTROY_AND_FREE_OBJECTS( _HUD->logos );
        DESTROY_AND_FREE_OBJECTS( _HUD->hpGauges );
        DESTROY_AND_FREE_OBJECTS( _HUD->hpBars );
        DESTROY_AND_FREE_OBJECTS( _HUD->names );
        DESTROY_AND_FREE_OBJECTS( _HUD->meterGauges );
        DESTROY_AND_FREE_OBJECTS( _HUD->meterBars );

#undef DESTROY_AND_FREE_OBJECTS

        // Timer
        {
            l_returnValue = object_t$destroy( &( _HUD->timer ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue = object_t$destroy( &( _HUD->timerBackground ) );

            if ( UNLIKELY( !l_returnValue ) ) {
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
        goto EXIT;
    }

    if ( UNLIKELY( !_HUD ) || UNLIKELY( !_HUD->name ) ||
         UNLIKELY( !_HUD->folder ) || UNLIKELY( !_HUD->extension ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_subFolder ) ) {
        goto EXIT;
    }

    {
        // TODO: Improve
        {
            char* l_folder = duplicateString( "/" );

            concatBeforeAndAfterString( &l_folder, _HUD->folder, _subFolder );

            {
                char* l_boxesPath = NULL;

                // Boxes
                {
                    l_boxesPath = duplicateString( _HUD->folder );

                    concatBeforeAndAfterString( &l_boxesPath, "/", ".boxes" );
                    concatBeforeAndAfterString( &l_boxesPath, l_folder, NULL );
                }

                char** l_animation = NULL;

                // Animation
                {
                    char* l_glob = duplicateString( "*." );

                    concatBeforeAndAfterString( &l_glob, _HUD->folder,
                                                _HUD->extension );

                    char* l_directory = duplicateString( l_folder );

                    concatBeforeAndAfterString(
                        &l_directory, asset_t$loader$assetsDirectory$get(),
                        NULL );

                    l_animation = getPathsByGlob( l_glob, l_directory );

                    free( l_glob );
                    free( l_directory );

                    FOR_ARRAY( char**, l_animation ) {
                        concatBeforeAndAfterString( _element, "/", NULL );
                        concatBeforeAndAfterString( _element, l_folder, NULL );
                    }
                }

                l_returnValue = object_t$state$add$fromFiles(
                    _element, _renderer, l_boxesPath, l_animation, false,
                    true );

                free( l_boxesPath );

                FREE_ARRAY_ELEMENTS( l_animation );
                FREE_ARRAY( l_animation );
            }

            free( l_folder );
        }

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
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
        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_amount ) ) {
        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )info, "Loading HUD: '%s'\n",
                                      _HUD->name );

#define TRY_LOAD_ALL_OR_EXIT( _field )                                        \
    do {                                                                      \
        FOR_ARRAY( object_t* const*, _HUD->_field ) {                         \
            object_t l_element = object_t$create();                           \
            l_returnValue = HUD_t$element$load$one( &l_element, _HUD,         \
                                                    _renderer, #_field "/" ); \
            if ( UNLIKELY( !l_returnValue ) ) {                               \
                goto EXIT;                                                    \
            }                                                                 \
            insertIntoArray( &( _HUD->_field ), clone( &l_element ) );        \
        }                                                                     \
    } while ( 0 )

        // TODO: Fix
        // TRY_LOAD_ALL_OR_EXIT( logos );

#undef TRY_LOAD_ALL_OR_EXIT

        // Timer
        {
            l_returnValue = HUD_t$element$load$one( &( _HUD->timer ), _HUD,
                                                    _renderer, "timer/" );

            if ( UNLIKELY( !l_returnValue ) ) {
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
        goto EXIT;
    }

    {
#define REMOVE_STATES_IN_OBJECTS( _objects )                     \
    do {                                                         \
        FOR_ARRAY( object_t* const*, _objects ) {                \
            l_returnValue = object_t$states$remove( *_element ); \
            if ( UNLIKELY( !l_returnValue ) ) {                  \
                goto EXIT;                                       \
            }                                                    \
        }                                                        \
    } while ( 0 )

        REMOVE_STATES_IN_OBJECTS( _HUD->logos );
        REMOVE_STATES_IN_OBJECTS( _HUD->hpGauges );
        REMOVE_STATES_IN_OBJECTS( _HUD->hpBars );
        REMOVE_STATES_IN_OBJECTS( _HUD->names );
        REMOVE_STATES_IN_OBJECTS( _HUD->meterGauges );
        REMOVE_STATES_IN_OBJECTS( _HUD->meterBars );

#undef REMOVE_STATES_IN_OBJECTS

        // Timer
        {
            l_returnValue = object_t$states$remove( &( _HUD->timer ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue =
                object_t$states$remove( &( _HUD->timerBackground ) );

            if ( UNLIKELY( !l_returnValue ) ) {
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
        goto EXIT;
    }

    {
#define STEP_OBJECTS( _objects )                              \
    do {                                                      \
        FOR_ARRAY( object_t* const*, _objects ) {             \
            l_returnValue = object_t$step( *_element, 0, 0 ); \
            if ( UNLIKELY( !l_returnValue ) ) {               \
                goto EXIT;                                    \
            }                                                 \
        }                                                     \
    } while ( 0 )

        STEP_OBJECTS( _HUD->logos );
        STEP_OBJECTS( _HUD->hpGauges );
        STEP_OBJECTS( _HUD->hpBars );
        STEP_OBJECTS( _HUD->names );
        STEP_OBJECTS( _HUD->meterGauges );
        STEP_OBJECTS( _HUD->meterBars );

#undef STEP_OBJECTS

        // Timer
        {
            l_returnValue = object_t$step( &( _HUD->timer ), 0, 0 );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue = object_t$step( &( _HUD->timerBackground ), 0, 0 );

            if ( UNLIKELY( !l_returnValue ) ) {
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
        goto EXIT;
    }

    {
        const SDL_FRect l_cameraRectangle = { .x = 0, .y = 0, .w = 0, .h = 0 };
        const bool l_doDrawBoxes = false;

#define RENDER_OBJECTS( _objects )                                          \
    do {                                                                    \
        FOR_ARRAY( object_t* const*, ( _objects ) ) {                       \
            l_returnValue = object_t$render( *_element, &l_cameraRectangle, \
                                             l_doDrawBoxes );               \
            if ( UNLIKELY( !l_returnValue ) ) {                             \
                goto EXIT;                                                  \
            }                                                               \
        }                                                                   \
    } while ( 0 )

        RENDER_OBJECTS( _HUD->logos );
        RENDER_OBJECTS( _HUD->hpGauges );
        RENDER_OBJECTS( _HUD->hpBars );
        RENDER_OBJECTS( _HUD->names );
        RENDER_OBJECTS( _HUD->meterGauges );
        RENDER_OBJECTS( _HUD->meterBars );

#undef RENDER_OBJECTS

        // Timer
        {
            l_returnValue = object_t$render(
                &( _HUD->timer ), &l_cameraRectangle, l_doDrawBoxes );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        // Timer background
        {
            l_returnValue = object_t$render(
                &( _HUD->timer ), &l_cameraRectangle, l_doDrawBoxes );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
