#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <argp.h>
#include <stdlib.h>

#include "FPS.h"
#include "applicationState_t.h"
#include "asset_t.h"
#include "config_t.h"
#include "log.h"
#include "stdfunc.h"
#include "vsync.h"

#define LOG_FILE_NAME_DEFAULT "log"
#define LOG_FILE_EXTENSION_DEFAULT "txt"

#define ASSETS_DIRECTORY "assets"

#define SETTINGS_FILE_NAME "settings"
#define SETTINGS_FILE_EXTENSION "ini"

#define CONFIG_FILE_NAME "config"
#define CONFIG_FILE_EXTENSION "ini"

#define REQUIRED_ARGUMENT_COUNT 1
#define MAX_ARGUMENT_COUNT 1

const char* argp_program_version;
const char* argp_program_bug_address;

struct arguments {
    char* filename;
    char* output_file;
};

static error_t parse_opt( int key, char* arg, struct argp_state* state ) {
    error_t l_returnValue = 0;

    struct arguments* arguments = state->input;

    switch ( key ) {
        case 'o': {
            arguments->output_file = arg;

            break;
        }

        case 'v': {
            const logLevel_t l_logLevel = info;

            if ( UNLIKELY( !log$level$set( l_logLevel ) ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Setting log level to %s\n",
                    log$level$convert$toString( l_logLevel ) );

                l_returnValue = EPERM;

                goto EXIT;
            }

            break;
        }

        case 'q': {
            const logLevel_t l_logLevel = unknownLogLevel;

            if ( UNLIKELY( !log$level$set( l_logLevel ) ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Setting log level to %s\n",
                    log$level$convert$toString( l_logLevel ) );

                l_returnValue = EPERM;

                goto EXIT;
            }

            break;
        }

        case ARGP_KEY_ARG: {
            if ( state->arg_num >= MAX_ARGUMENT_COUNT ) {
                argp_usage( state ); // too many args
            }

            arguments->filename = arg;

            break;
        }

        case ARGP_KEY_END: {
            if ( state->arg_num < REQUIRED_ARGUMENT_COUNT ) {
                argp_usage( state ); // missing filename
            }

            break;
        }

        default: {
            return ( ARGP_ERR_UNKNOWN );
        }
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool parseArguments(
    applicationState_t* restrict _applicationState,
    int _argumentCount,
    char** restrict _argumentVector ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_argumentVector ) ) {
        goto EXIT;
    }

    {
        // Application name and version
        {
            char* l_nameAndVersion = duplicateString( " " );

            concatBeforeAndAfterString( &l_nameAndVersion,
                                        _applicationState->settings.identifier,
                                        _applicationState->settings.version );

            argp_program_version = l_nameAndVersion;
        }

        argp_program_bug_address = _applicationState->settings.contactAddress;

        char* l_description = NULL;

        {
            l_description = duplicateString( " - " );

            concatBeforeAndAfterString(
                &l_description, _applicationState->settings.identifier,
                _applicationState->settings.description );
        }

        // Command-line options
        struct argp_option options[] = {
            { "output", 'o', "FILE", 0, "Output to FILE", 0 },
            { "verbose", 'v', 0, 0, "Produce verbose output", 0 },
            { "quiet", 'q', 0, 0, "Don not produce any output", 0 },
            { 0 } };

        // [NAME] - optional
        // NAME - required
        // NAME... - at least one and more
        const char l_arguments[] = "";

        struct argp argp = { options, parse_opt, l_arguments, l_description };

        struct arguments arguments = { 0 };

        argp_parse( &argp, _argumentCount, _argumentVector, 0, 0, &arguments );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool init( applicationState_t* restrict _applicationState,
                               int _argumentCount,
                               char** restrict _argumentVector ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_argumentVector ) ) {
        goto EXIT;
    }

    {
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

            // Configuration
            {
                // Backgrounds
                // TODO: Implement
                // UI
                // Characters
                if ( UNLIKELY( !config_t$load$fromPath(
                         &( _applicationState->config ), CONFIG_FILE_NAME,
                         CONFIG_FILE_EXTENSION ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading config\n" );

                    goto EXIT;
                }
            }

            // Application arguments
            // Setup recources to load
            {
                if ( UNLIKELY( !parseArguments( _applicationState,
                                                _argumentCount,
                                                _argumentVector ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Parsing arguments\n" );

                    goto EXIT;
                }
            }

            // Load resources
            {
                if ( UNLIKELY(
                         !applicationState_t$load( _applicationState ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading application state\n" );

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

        // Gamepad
        {
            l_returnValue = !( SDL_HasGamepad() );

            if ( !l_returnValue ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing Gamepad\n" );

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
