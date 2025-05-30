#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <stdbool.h>
#include <stddef.h>

#include "applicationState_t.h"
#include "log.h"
#include "stdfunc.h"

typedef SDL_Event event_t;

static FORCE_INLINE bool windowResize(
    applicationState_t* restrict _applicationState,
    event_t* restrict _event ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_event ) ) {
        goto EXIT;
    }

    {
        static size_t l_lastResizeFrameIndex = 0;
        const size_t l_totalFramesRendered =
            _applicationState->totalFramesRendered;

        if ( l_lastResizeFrameIndex < l_totalFramesRendered ) {
            const float l_newWidth = ( float )( _event->window.data1 );
            const float l_newHeight = ( float )( _event->window.data2 );
            const float l_logicalWidth = _applicationState->logicalWidth;
            const float l_logicalHeigth = _applicationState->logicalHeight;

            const float l_scaleX = ( l_newWidth / l_logicalWidth );
            const float l_scaleY = ( l_newHeight / l_logicalHeigth );

            if ( !SDL_SetRenderScale( _applicationState->renderer, l_scaleX,
                                      l_scaleY ) ) {
                l_returnValue = false;

                log$transaction$query$format( ( logLevel_t )error,
                                              "Setting render scale: '%s'\n",
                                              SDL_GetError() );

                goto EXIT;
            }
        }

        l_lastResizeFrameIndex = l_totalFramesRendered;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool event( applicationState_t* _applicationState,
                                event_t* _event ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    {
        ( void )( sizeof( _applicationState ) );

        switch ( _event->type ) {
            case ( SDL_EVENT_QUIT ): {
                l_returnValue = false;

                goto EXIT;
            }

            case SDL_EVENT_WINDOW_RESIZED: {
                windowResize( _applicationState, _event );

                break;
            }

            default: {
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

SDL_AppResult SDL_AppEvent( void* _applicationState, SDL_Event* _event ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    event_t* l_event = ( event_t* )_event;

    if ( UNLIKELY( !event( l_applicationState, l_event ) ) ) {
        goto EXIT;
    }

    l_returnValue = SDL_APP_CONTINUE;

EXIT:
    return ( l_returnValue );
}
