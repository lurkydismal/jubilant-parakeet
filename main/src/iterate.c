#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include "applicationState_t.h"

SDL_AppResult SDL_AppIterate( void* _applicationState ) {
    SDL_AppResult l_returnValue = SDL_APP_CONTINUE;

    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    SDL_RenderClear( l_applicationState->renderer );

    SDL_RenderPresent( l_applicationState->renderer );

    ( l_applicationState->totalFrameCount )++;

    SDL_Delay( l_applicationState->iterateSleepTime );

    return ( l_returnValue );
}
