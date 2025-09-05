#include "iterate.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "FPS.h"
#include "log.h"
#include "stdfunc.h"

bool iterate( applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = vsync$begin();

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Vsync begin" );

            goto EXIT;
        }

#if defined( DEBUG )
        // Hot reload
        {
#define TRY_CHECK_WATCH_OR_EXIT( _field )                                  \
    do {                                                                   \
        FOR_ARRAY( watch_t* const*, _applicationState->_field->watches ) { \
            l_returnValue = watch_t$check( *_element, false );             \
            if ( UNLIKELY( !l_returnValue ) ) {                            \
                log$transaction$query( ( logLevel_t )error,                \
                                       "Checking " #_field " watch" );     \
                goto EXIT;                                                 \
            }                                                              \
        }                                                                  \
    } while ( 0 )

#if 0
            TRY_CHECK_WATCH_OR_EXIT( background );
#endif
            // TRY_CHECK_WATCH_OR_EXIT( HUD );
            // TRY_CHECK_WATCH_OR_EXIT( character );

#undef TRY_CHECK_WATCH_OR_EXIT
        }
#endif

        if ( !_applicationState->isPaused ) {
#if 0
            camera_t$update( &( _applicationState->camera ),
                             &( _applicationState->localPlayer ) );
#endif
        }

        // Render
        {
            SDL_RenderClear( _applicationState->renderer );

            // Background
            {
#if 0
                l_returnValue = background_t$render(
                    _applicationState->background,
                    &( _applicationState->camera.rectangle ), false );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Rendering background" );

                    goto EXIT;
                }
#endif
            }

#if 0
            // HUD
            {
                l_returnValue = HUD_t$render( _applicationState->HUD );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Rendering HUD" );

                    goto EXIT;
                }
            }
#endif

            // TODO: Players

            SDL_RenderPresent( _applicationState->renderer );
        }

        if ( !_applicationState->isPaused ) {
            // Step
            {
                // Background
                {
#if 0
                    l_returnValue =
                        background_t$step( _applicationState->background );

                    if ( UNLIKELY( !l_returnValue ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Stepping background" );

                        goto EXIT;
                    }
#endif
                }

#if 0
                // HUD
                {
                    l_returnValue = HUD_t$step( _applicationState->HUD );

                    if ( UNLIKELY( !l_returnValue ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Stepping HUD" );

                        goto EXIT;
                    }
                }
#endif

                // TODO: Players
            }
        }

        l_returnValue = vsync$end();

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Vsync end" );

            goto EXIT;
        }

        if ( ( _applicationState->totalFramesRendered % 60 ) == 0 ) {
            log$transaction$commit();
        }

        ( _applicationState->totalFramesRendered )++;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
