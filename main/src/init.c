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

#define LOG_FILE_NAME_DEFAULT "log"
#define LOG_FILE_EXTENSION_DEFAULT "txt"

#define ASSETS_DIRECTORY "assets"

#define SETTINGS_FILE_NAME "settings"
#define SETTINGS_FILE_EXTENSION "ini"

SDL_AppResult SDL_AppInit( void** _applicationState,
                           int _argumentCount,
                           char** _argumentVector ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    ( void )( sizeof( _argumentCount ) );
    ( void )( sizeof( _argumentVector ) );

    {
        SDL_SetAppMetadata( "jubilant-parakeet", "0.1",
                            "com.github.jubilant-parakeet" );

        // Init SDL sub-systems
        {
            SDL_Init( SDL_INIT_VIDEO );
        }

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

        // Generate application state
        {
            // TODO: Free on error
            applicationState_t* l_applicationState =
                ( applicationState_t* )malloc( sizeof( applicationState_t ) );

            *l_applicationState = applicationState_t$create();

            // Asset loader
            {
                if ( UNLIKELY( !asset_t$loader$init( ASSETS_DIRECTORY ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Initializing asset loader\n" );

                    goto EXIT;
                }
            }

            // Settings
            {
                if ( UNLIKELY( !settings_t$load$fromPath(
                         &( l_applicationState->settings ), SETTINGS_FILE_NAME,
                         SETTINGS_FILE_EXTENSION ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading settings\n" );

                    log$transaction$query( ( logLevel_t )info,
                                           "Loading default settings\n" );

                    l_applicationState->settings = settings_t$create();
                }
            }

            // Window and Renderer
            {
                if ( UNLIKELY( !SDL_CreateWindowAndRenderer(
                         l_applicationState->settings.window.name,
                         l_applicationState->settings.window.height,
                         l_applicationState->settings.window.width, 0,
                         &( l_applicationState->window ),
                         &( l_applicationState->renderer ) ) ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error,
                        "Window or Renderer creation: '%s'\n", SDL_GetError() );

                    goto EXIT;
                }
            }

            // Scaling
#if 0
            // TODO: Fix
            {
                float l_scaleX =
                    ( ( float )( l_applicationState->settings.window.width ) /
                      ( float )( l_applicationState->logicalWidth ) );
                float l_scaleY =
                    ( ( float )( l_applicationState->settings.window.height ) /
                      ( float )( l_applicationState->logicalHeight ) );

                if ( !SDL_SetRenderScale( l_applicationState->renderer,
                                          l_scaleX, l_scaleY ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error, "Setting render scale: '%s'\n",
                        SDL_GetError() );

                    goto EXIT;
                }
            }
#endif

            // Vsync
            {
                if ( UNLIKELY( !vsync$init(
                         l_applicationState->settings.window.vsync,
                         l_applicationState->settings.window.desiredFPS,
                         l_applicationState->renderer ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Initializing Vsync\n" );

                    goto EXIT;
                }
            }

            // FPS
            {
                if ( UNLIKELY( !FPS$init(
                         &l_applicationState->totalFramesRendered ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Initializing FPS\n" );

                    goto EXIT;
                }
            }

            *_applicationState = l_applicationState;
        }

        l_returnValue = SDL_APP_CONTINUE;
    }

EXIT:
    return ( l_returnValue );
}
