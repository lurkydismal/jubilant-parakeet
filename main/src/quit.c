#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "applicationState_t.h"
#include "log.h"

void SDL_AppQuit( void* _applicationState, SDL_AppResult _appRunResult ) {
    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    if ( LIKELY( _applicationState ) ) {
        if ( LIKELY( l_applicationState->renderer ) ) {
            SDL_DestroyRenderer( l_applicationState->renderer );
        }

        if ( LIKELY( l_applicationState->window ) ) {
            SDL_DestroyWindow( l_applicationState->window );
        }

        if ( UNLIKELY( !applicationState_t$destroy( l_applicationState ) ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Destroying application state\n" );
        }

        free( _applicationState );
    }

    if ( _appRunResult != SDL_APP_SUCCESS ) {
        log$transaction$query$format( ( logLevel_t )error,
                                      "Application exited with %u: '%s'\n",
                                      _appRunResult, SDL_GetError() );
    }
}
