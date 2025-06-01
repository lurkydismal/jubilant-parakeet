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
        goto EXIT;
    }

    {
        static bool x = false;

        if ( !x ) {
            x = true;

            // bg
            {
                char** files = createArray( char* );

                insertIntoArray( &files, "test.boxes" );
                insertIntoArray( &files, "test_1280x720_1-2.png" );

                bool ret = object_t$state$add$fromFiles(
                    &( _applicationState->background ),
                    _applicationState->renderer, files, false, false );

                FREE_ARRAY( files );

                if ( !ret ) {
                    goto EXIT;
                }
            }

            {
                char** files = createArray( char* );

                insertIntoArray( &files, "test1.boxes" );
                insertIntoArray( &files, "test_100x100_1-2.png" );

                bool ret = player_t$state$add$fromFiles(
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
        }

        if ( UNLIKELY( !vsync$begin() ) ) {
            l_returnValue = false;

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
                l_returnValue = object_t$render(
                    &( _applicationState->background ),
                    &( _applicationState->camera.rectangle ), false );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }
            }

            if ( x ) {
                bool ret = player_t$render(
                    &( _applicationState->localPlayer ),
                    &( _applicationState->camera.rectangle ), true );

                if ( !ret ) {
                    goto EXIT;
                }
            }

            SDL_RenderPresent( _applicationState->renderer );
        }

        // Step
        {
            // Background
            {
                l_returnValue =
                    object_t$step( &( _applicationState->background ), 0, 0 );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }
            }
        }

        if ( UNLIKELY( !vsync$end() ) ) {
            l_returnValue = false;

            log$transaction$query( ( logLevel_t )error, "Vsync end\n" );

            goto EXIT;
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
