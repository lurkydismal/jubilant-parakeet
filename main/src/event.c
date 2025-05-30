#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <stdbool.h>
#include <stddef.h>

#include "applicationState_t.h"
#include "log.h"
#include "stdfunc.h"

typedef SDL_Event event_t;

static FORCE_INLINE bool onWindowResize(
    applicationState_t* restrict _applicationState,
    float _width,
    float _height ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    {
        static size_t l_lastResizeFrameIndex = 0;
        const size_t l_totalFramesRendered =
            _applicationState->totalFramesRendered;

        if ( l_lastResizeFrameIndex < l_totalFramesRendered ) {
            const float l_logicalWidth = _applicationState->logicalWidth;
            const float l_logicalHeigth = _applicationState->logicalHeight;

            const float l_scaleX = ( _width / l_logicalWidth );
            const float l_scaleY = ( _height / l_logicalHeigth );

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

static FORCE_INLINE bool onKey( applicationState_t* restrict _applicationState,
                                SDL_Scancode _scancode ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    {
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
        switch ( _event->type ) {
            case ( SDL_EVENT_QUIT ): {
                l_returnValue = false;

                goto EXIT;
            }

            case SDL_EVENT_WINDOW_RESIZED: {
                float l_newWidth = _event->window.data1;
                float l_newHeight = _event->window.data2;

                onWindowResize( _applicationState, l_newHeight, l_newWidth );

                break;
            }

            case SDL_EVENT_KEY_DOWN: {
                onKey( _applicationState, _event->key.scancode );

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
