#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <stdbool.h>
#include <stddef.h>

#include "applicationState_t.h"
#include "controls_t.h"
#include "inputBuffer_t.h"
#include "log.h"
#include "stdfunc.h"

typedef SDL_Event event_t;

static FORCE_INLINE bool onWindowResize(
    applicationState_t* restrict _applicationState,
    float _width,
    float _height ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        static size_t l_lastResizeFrame = 0;
        const size_t l_totalFramesRendered =
            _applicationState->totalFramesRendered;

        if ( l_lastResizeFrame < l_totalFramesRendered ) {
            const float l_logicalWidth = _applicationState->logicalWidth;
            const float l_logicalHeigth = _applicationState->logicalHeight;

            const float l_scaleX = ( _width / l_logicalWidth );
            const float l_scaleY = ( _height / l_logicalHeigth );

            if ( !SDL_SetRenderScale( _applicationState->renderer, l_scaleX,
                                      l_scaleY ) ) {
                l_returnValue = false;

                log$transaction$query$format( ( logLevel_t )error,
                                              "Setting render scale: '%s'",
                                              SDL_GetError() );

                goto EXIT;
            }
        }

        l_lastResizeFrame = l_totalFramesRendered;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool onKey( applicationState_t* restrict _applicationState,
                                SDL_Scancode _scancode ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_scancode ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        static size_t l_lastInputFrame = 0;
        const size_t l_totalFramesRendered =
            _applicationState->totalFramesRendered;

        if ( l_lastInputFrame < l_totalFramesRendered ) {
            input_t l_input = 0;

            {
                int l_keysAmount = 0;
                const bool* l_keysState = SDL_GetKeyboardState( &l_keysAmount );

                FOR_RANGE( int, 0, l_keysAmount ) {
                    // If pressed
                    if ( l_keysState[ _index ] ) {
                        SDL_Scancode l_scancode = _index;

                        const control_t* l_control =
                            controls_t$control_t$convert$fromScancode(
                                &( _applicationState->settings.controls ),
                                l_scancode );

                        if ( l_control ) {
                            l_input |= l_control->input;
                        }
                    }
                }
            }

            if ( l_input ) {
                l_returnValue =
                    player_t$input$add( &( _applicationState->localPlayer ),
                                        l_input, l_totalFramesRendered );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Adding player input" );

                    goto EXIT;
                }
            }
        }

        l_lastInputFrame = l_totalFramesRendered;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool event( applicationState_t* _applicationState,
                                event_t* _event ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        switch ( _event->type ) {
            case ( SDL_EVENT_QUIT ): {
                l_returnValue = false;

                goto EXIT;
            }

            case SDL_EVENT_WINDOW_RESIZED: {
                const float l_newWidth = _event->window.data1;
                const float l_newHeight = _event->window.data2;

                l_returnValue = onWindowResize( _applicationState, l_newWidth,
                                                l_newHeight );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Handling window resize" );

                    goto EXIT;
                }

                break;
            }

            case SDL_EVENT_KEY_DOWN: {
                l_returnValue =
                    onKey( _applicationState, _event->key.scancode );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Handling key press" );

                    goto EXIT;
                }

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
