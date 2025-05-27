#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

#include "applicationState_t.h"
#include "log.h"

SDL_AppResult SDL_AppEvent( void* _applicationState, SDL_Event* _event ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    ( void )( sizeof( l_applicationState ) );

    if ( _event->type == SDL_EVENT_QUIT ) {
        l_returnValue = SDL_APP_SUCCESS;

        goto EXIT;
    }

    l_returnValue = SDL_APP_CONTINUE;

EXIT:
    return ( l_returnValue );
}
