#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include "FPS.h"
#include "applicationState_t.h"
#include "asset_t.h"
#include "log.h"
#include "stdfunc.h"
#include "vsync.h"

#define REQUIRED_ARGUMENT_COUNT 1

#define LOG_FILE_NAME_DEFAULT "log"
#define LOG_FILE_EXTENSION_DEFAULT "txt"

#define ASSETS_DIRECTORY "assets"

#define SETTINGS_FILE_NAME "settings"
#define SETTINGS_FILE_EXTENSION "ini"

static FORCE_INLINE bool init( applicationState_t* restrict _applicationState,
                               int _argumentCount,
                               char** restrict _argumentVector ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( _argumentCount < REQUIRED_ARGUMENT_COUNT ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_argumentVector ) ) {
        goto EXIT;
    }

    {
        ( void )( sizeof( _argumentCount ) );
        ( void )( sizeof( _argumentVector ) );

        // Log
        {
            if ( UNLIKELY( !log$init( LOG_FILE_NAME_DEFAULT,
                                      LOG_FILE_EXTENSION_DEFAULT ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing logging system\n" );

                goto EXIT;
            }

#if defined( DEBUG )

            const logLevel_t l_logLevel = debug;

#elif defined( PROFILE )

            const logLevel_t l_logLevel = info;

#endif

#if ( defined( DEBUG ) || defined( PROFILE ) )

            if ( UNLIKELY( !log$level$set( l_logLevel ) ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Setting log level to %s\n",
                    log$level$convert$toString( l_logLevel ) );

                goto EXIT;
            }

#endif
        }

        // Asset loader
        {
            if ( UNLIKELY( !asset_t$loader$init( ASSETS_DIRECTORY ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing asset loader\n" );

                goto EXIT;
            }
        }

        // Generate application state
        {
            *_applicationState = applicationState_t$create();

            // Settings
            {
                if ( UNLIKELY( !settings_t$load$fromPath(
                         &( _applicationState->settings ), SETTINGS_FILE_NAME,
                         SETTINGS_FILE_EXTENSION ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading settings\n" );

                    log$transaction$query( ( logLevel_t )info,
                                           "Loading default settings\n" );

                    _applicationState->settings = settings_t$create();
                }
            }

            // Metadata
            {
                l_returnValue = SDL_SetAppMetadata(
                    _applicationState->settings.window.name,
                    _applicationState->settings.version,
                    _applicationState->settings.identifier );

                log$transaction$query$format(
                    ( logLevel_t )info,
                    "Window name: '%s', Version: '%s', Identifier: '%s'\n",
                    _applicationState->settings.window.name,
                    _applicationState->settings.version,
                    _applicationState->settings.identifier );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error, "Setting render scale: '%s'\n",
                        SDL_GetError() );

                    goto EXIT;
                }
            }

            // Init SDL sub-systems
            {
                SDL_Init( SDL_INIT_VIDEO );
            }

            // Window and Renderer
            {
                if ( UNLIKELY( !SDL_CreateWindowAndRenderer(
                         _applicationState->settings.window.name,
                         _applicationState->settings.window.width,
                         _applicationState->settings.window.height,
                         ( SDL_WINDOW_INPUT_FOCUS ),
                         &( _applicationState->window ),
                         &( _applicationState->renderer ) ) ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error,
                        "Window or Renderer creation: '%s'\n", SDL_GetError() );

                    goto EXIT;
                }
            }
        }

        // Default scale mode
        {
            l_returnValue = SDL_SetDefaultTextureScaleMode(
                _applicationState->renderer, SDL_SCALEMODE_NEAREST );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Setting render scale mode: '%s'\n",
                    SDL_GetError() );

                goto EXIT;
            }
        }

        // Scaling
        {
            const float l_scaleX =
                ( ( float )( _applicationState->settings.window.width ) /
                  ( float )( _applicationState->logicalWidth ) );
            const float l_scaleY =
                ( ( float )( _applicationState->settings.window.height ) /
                  ( float )( _applicationState->logicalHeight ) );

            if ( !SDL_SetRenderScale( _applicationState->renderer, l_scaleX,
                                      l_scaleY ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Setting render scale: '%s'\n",
                                              SDL_GetError() );

                goto EXIT;
            }
        }

        // Vsync
        {
            if ( UNLIKELY(
                     !vsync$init( _applicationState->settings.window.vsync,
                                  _applicationState->settings.window.desiredFPS,
                                  _applicationState->renderer ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing Vsync\n" );

                goto EXIT;
            }
        }

        // FPS
        {
            if ( UNLIKELY( !FPS$init(
                     &( _applicationState->totalFramesRendered ) ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing FPS\n" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

SDL_AppResult SDL_AppInit( void** _applicationState,
                           int _argumentCount,
                           char** _argumentVector ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    {
        applicationState_t* l_applicationState =
            ( applicationState_t* )malloc( sizeof( applicationState_t ) );

        __builtin_memset( l_applicationState, 0, sizeof( applicationState_t ) );

        if ( UNLIKELY( !init( l_applicationState, _argumentCount,
                              _argumentVector ) ) ) {
            free( l_applicationState );

            goto EXIT;
        }

        *_applicationState = l_applicationState;

        l_returnValue = SDL_APP_CONTINUE;
    }

EXIT:
    return ( l_returnValue );
}
