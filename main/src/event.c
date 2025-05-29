#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>

#include "applicationState_t.h"
#include "log.h"

SDL_AppResult SDL_AppEvent( void* _applicationState, SDL_Event* _event ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    ( void )( sizeof( l_applicationState ) );

    switch ( _event->type ) {
        case ( SDL_EVENT_QUIT ): {
            l_returnValue = SDL_APP_SUCCESS;

            goto EXIT;
        }

        case SDL_EVENT_WINDOW_RESIZED: {
            static size_t l_lastResize = 0;

            // TODO: Refactor
            if ( l_lastResize < l_applicationState->totalFramesRendered ) {
                int l_newWidth = _event->window.data1;
                int l_newHeight = _event->window.data2;
                float l_scaleX = ( float )l_newWidth /
                                 ( float )l_applicationState->logicalWidth;
                float l_scaleY = ( float )l_newHeight /
                                 ( float )l_applicationState->logicalHeight;

                if ( !SDL_SetRenderScale( l_applicationState->renderer,
                                          l_scaleX, l_scaleY ) ) {
                    l_returnValue = SDL_APP_FAILURE;

                    log$transaction$query$format(
                        ( logLevel_t )error, "Setting render scale: '%s'\n",
                        SDL_GetError() );

                    goto EXIT;
                }
            }

            l_lastResize = l_applicationState->totalFramesRendered;

            break;
        }

        default: {
        }
    }

    l_returnValue = SDL_APP_CONTINUE;

EXIT:
    return ( l_returnValue );
}
