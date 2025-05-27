#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "applicationState_t.h"
#include "log.h"
#include "stdfunc.h"

SDL_AppResult SDL_AppIterate( void* _applicationState ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    if ( UNLIKELY( !vsync$begin() ) ) {
        goto EXIT;
    }

    // Render
    {
        SDL_RenderClear( l_applicationState->renderer );

        SDL_RenderPresent( l_applicationState->renderer );
    }

    if ( UNLIKELY( !vsync$end() ) ) {
        goto EXIT;
    }

    ( l_applicationState->totalFramesRendered )++;

    l_returnValue = SDL_APP_CONTINUE;

EXIT:
    return ( l_returnValue );
}
