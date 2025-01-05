#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include "applicationState_t.h"

Uint32 SDLCALL FPSCountAndShow( void* _totalFrameCount,
                                SDL_TimerID _timerId,
                                Uint32 _interval ) {
    static size_t l_previousTotalFrameCount = 0;

    size_t l_currentTotalFrameCount = *( ( size_t* )( _totalFrameCount ) );

    size_t l_framesElapsed =
        ( l_currentTotalFrameCount - l_previousTotalFrameCount );

    SDL_Log( "FPS: %u\n", l_framesElapsed );

    l_previousTotalFrameCount = l_currentTotalFrameCount;

    return ( _interval );
}

void SDLCALL iterateIntervalHintCallback( void* _iterateSleepTime,
                                          const char* _hintName,
                                          const char* _hintOldValue,
                                          const char* _hintNewValue ) {
    if ( _hintNewValue ) {
        Uint16* l_iterateSleepTime = ( Uint16* )_iterateSleepTime;

        const Uint16 l_oneSecond = 1000;
        const Uint16 l_hintNewValue = SDL_atoi( _hintNewValue );

        *l_iterateSleepTime = ( l_oneSecond / l_hintNewValue );
    }
}

SDL_AppResult SDL_AppInit( void** _applicationState,
                           int _arcgumentCount,
                           char** _argumentVector ) {
    SDL_AppResult l_returnValue = SDL_APP_CONTINUE;

    SDL_SetAppMetadata( "fgengine", "0.1", "com.github.fgengine" );

    // Init sub-systems
    { SDL_Init( SDL_INIT_VIDEO ); }

    // Generate application state
    {
        applicationState_t l_applicationState;

        l_applicationState.totalFrameCount = 0;
        l_applicationState.iterateSleepTime = ( 1000 / 60 );

        SDL_CreateWindowAndRenderer( "fgengine", 640, 480, 0,
                                     &( l_applicationState.window ),
                                     &( l_applicationState.renderer ) );

        {
            *_applicationState = SDL_malloc( sizeof( applicationState_t ) );

            SDL_memcpy( *_applicationState, &l_applicationState,
                        sizeof( applicationState_t ) );
        }
    }

    {
        // Register iterate loop rate hint callback
        {
            applicationState_t* l_applicationState =
                ( applicationState_t* )( *_applicationState );

            Uint16* l_iterateSleepTime =
                &( l_applicationState->iterateSleepTime );

            SDL_AddHintCallback( SDL_HINT_MAIN_CALLBACK_RATE,
                                 iterateIntervalHintCallback,
                                 l_iterateSleepTime );
        }

        // Register FPS counter timer
        {
            applicationState_t* l_applicationState =
                ( applicationState_t* )( *_applicationState );

            size_t* l_totalFrameCount = &( l_applicationState->totalFrameCount );

            SDL_AddTimer( 1000, FPSCountAndShow, l_totalFrameCount );
        }
    }

    return ( l_returnValue );
}
