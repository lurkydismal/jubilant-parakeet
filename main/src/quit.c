#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#if defined( __SANITIZE_LEAK__ )

#include <sanitizer/lsan_interface.h>

#endif

#include <stdlib.h>

#include "FPS.h"
#include "applicationState_t.h"
#include "asset_t.h"
#include "log.h"
#include "stdfunc.h"
#include "vsync.h"

typedef enum { failure, success } result_t;

static FORCE_INLINE result_t
result_t$convert$fromSDL_AppResult( SDL_AppResult _appRunResult ) {
    switch ( _appRunResult ) {
        case SDL_APP_SUCCESS: {
            return ( ( result_t )success );
        }

        case SDL_APP_FAILURE: {
            return ( ( result_t )failure );
        }

        default: {
            return ( ( result_t )failure );
        }
    }
}

static FORCE_INLINE const char* result_t$convert$toStaticString(
    const result_t _result ) {
    switch ( _result ) {
        case success: {
            return ( "SUCCESS" );
        }

        default: {
            return ( "FAILURE" );
        }
    }
}

static FORCE_INLINE bool quit( applicationState_t* restrict _applicationState,
                               const result_t _result ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        // Report if SDL error occured before quitting
        {
            const char* l_SDLErrorMessage = SDL_GetError();

            if ( l_SDLErrorMessage[ 0 ] != '\0' ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Application exited with %s: '%s'\n",
                    result_t$convert$toStaticString( _result ),
                    l_SDLErrorMessage );
            }
        }

        // FPS
        {
            if ( UNLIKELY( !FPS$quit() ) ) {
                log$transaction$query( ( logLevel_t )error, "Quitting FPS\n" );

                goto EXIT;
            }
        }

        // Vsync
        {
            if ( UNLIKELY( !vsync$quit() ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Quitting Vsync\n" );

                goto EXIT;
            }
        }

        // Settings
        {
            if ( UNLIKELY( !settings_t$unload(
                     &( _applicationState->settings ) ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Unloading settings\n" );

                goto EXIT;
            }
        }

        // Asset loader
        {
            if ( UNLIKELY( !asset_t$loader$quit() ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Quitting asset loader\n" );

                goto EXIT;
            }
        }

        // Application state
        if ( LIKELY( _applicationState ) ) {
            if ( LIKELY( _applicationState->renderer ) ) {
                SDL_DestroyRenderer( _applicationState->renderer );
            }

            if ( LIKELY( _applicationState->window ) ) {
                SDL_DestroyWindow( _applicationState->window );
            }

            if ( UNLIKELY(
                     !applicationState_t$destroy( _applicationState ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying application state\n" );
            }

            free( _applicationState );
        }

        // TODO: Make application actually return success not on failure
        // Report application result
        if ( _result != ( result_t )success ) {
            const char* l_SDLErrorMessage = SDL_GetError();

            if ( l_SDLErrorMessage[ 0 ] != '\0' ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Application shutdown with %s: '%s'\n",
                    result_t$convert$toStaticString( _result ),
                    l_SDLErrorMessage );
            }
        }

        // Log
        {
            if ( UNLIKELY( !log$quit() ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Quitting logging system\n" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

void SDL_AppQuit( void* _applicationState, SDL_AppResult _appRunResult ) {
    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    const result_t l_result =
        result_t$convert$fromSDL_AppResult( _appRunResult );

    quit( l_applicationState, l_result );

    SDL_Quit();

#if defined( __SANITIZE_LEAK__ )

    __lsan_do_leak_check();

#endif
}
