#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#include "FPS.h"
#include "applicationState_t.h"
#include "log.h"
#include "player_t.h"
#include "stdfunc.h"

static FORCE_INLINE bool iterate(
    applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        static bool x = false;

        if ( !x ) {
            x = true;

#if 0
            {
                char** files = createArray( char* );

                insertIntoArray( &files, "test1.boxes" );
                insertIntoArray( &files, "test_100x100_1-2.png" );

                bool ret = player_t$state$add$fromPaths(
                    &( _applicationState->localPlayer ),
                    _applicationState->renderer, files, false, false );

                _applicationState->localPlayer.object.worldXMax =
                    ( _applicationState->logicalWidth - 100 );
                _applicationState->localPlayer.object.worldYMax =
                    ( _applicationState->logicalHeight - 100 );

                FREE_ARRAY( files );

                if ( !ret ) {
                    goto EXIT;
                }
            }
#endif
        }

        l_returnValue = vsync$begin();

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Vsync begin\n" );

            goto EXIT;
        }

        camera_t$update( &( _applicationState->camera ),
                         &( _applicationState->localPlayer ) );

        // Render
        {
            SDL_RenderClear( _applicationState->renderer );

            // Background
            {
                l_returnValue = background_t$render(
                    _applicationState->background,
                    &( _applicationState->camera.rectangle ), false );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Rendering background\n" );

                    goto EXIT;
                }
            }

#if 0
            if ( x ) {
                bool ret = player_t$render(
                    &( _applicationState->localPlayer ),
                    &( _applicationState->camera.rectangle ), true );

                if ( !ret ) {
                    goto EXIT;
                }
            }
#endif

            SDL_RenderPresent( _applicationState->renderer );
        }

        // Step
        {
            // Background
            {
                l_returnValue =
                    background_t$step( _applicationState->background );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Stepping background\n" );

                    goto EXIT;
                }
            }
        }

        l_returnValue = vsync$end();

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Vsync end\n" );

            goto EXIT;
        }

        // TODO: Fix
        input_t** l_inputs = player_t$inputsSequences$get$withLimit(
            &( _applicationState->localPlayer ),
            _applicationState->totalFramesRendered, 8 );

        if ( arrayLength( l_inputs ) ) {
            static bool asd = false;
            static char* l_result = NULL;

            if ( !asd ) {
                asd = true;

                l_result = ( char* )malloc( 100 );
            }

            size_t l_len = 0;

            FOR_ARRAY( input_t* const*, l_inputs ) {
                const char* s = input_t$convert$toStaticString( **_element );
                const size_t sl = __builtin_strlen( s );

                __builtin_memcpy( ( l_result + l_len ), s, sl );

                l_len += sl;
            }

            l_result[ l_len ] = '\0';

            log$transaction$query$format( ( logLevel_t )debug, "%s\n",
                                          l_result );
        }

        FREE_ARRAY( l_inputs );

        if ( ( _applicationState->totalFramesRendered % 10 ) == 0 ) {
            log$transaction$commit();
        }

        ( _applicationState->totalFramesRendered )++;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

SDL_AppResult SDL_AppIterate( void* _applicationState ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    if ( UNLIKELY( !iterate( l_applicationState ) ) ) {
        goto EXIT;
    }

    l_returnValue = SDL_APP_CONTINUE;

EXIT:
    return ( l_returnValue );
}
