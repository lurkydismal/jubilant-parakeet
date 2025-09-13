#pragma once

#include <SDL3/SDL_init.h>

#include <span>

#include "FPS.hpp"
#include "log.hpp"
#include "runtime.hpp"
#include "vsync.hpp"

namespace runtime {

template < size_t N >
    requires( N > 0 )
auto init( applicationState_t& _applicationState,
           [[maybe_unused]] std::span< std::string_view, N > _arguments )
    -> bool {
    bool l_returnValue = false;

    do {
        // Generate application state
        {
            // Metadata
            {
                logg::info(
                    "Window name: '{}', Version: '{}', Identifier: '{}'",
                    _applicationState.settings.window.name,
                    _applicationState.settings.version,
                    _applicationState.settings.identifier );

                if ( !SDL_SetAppMetadata(
                         std::string( _applicationState.settings.window.name )
                             .c_str(),
                         std::string( _applicationState.settings.version )
                             .c_str(),
                         std::string( _applicationState.settings.identifier )
                             .c_str() ) ) {
                    logg::error( "Setting render scale: '{}'", SDL_GetError() );

                    break;
                }
            }

            // TODO: Setup recources to load

            // Init SDL sub-systems
            SDL_Init( SDL_INIT_VIDEO );

            // Window and Renderer
            {
                if ( !SDL_CreateWindowAndRenderer(
                         std::string( _applicationState.settings.window.name )
                             .c_str(),
                         _applicationState.settings.window.width,
                         _applicationState.settings.window.height,
                         ( SDL_WINDOW_INPUT_FOCUS ),
                         &( _applicationState.window ),
                         &( _applicationState.renderer ) ) ) {
                    logg::error( "Window or Renderer creation: '{}'",
                                 SDL_GetError() );

                    break;
                }

#if 0
                _applicationState.width =
                    _applicationState.settings.window.width;
                _applicationState.height =
                    _applicationState.settings.window.width;

                logg::variable( _applicationState.width );
                logg::variable( _applicationState.height );
#endif
            }

            // Default scale mode
            {
                if ( !SDL_SetDefaultTextureScaleMode(
                         _applicationState.renderer,
                         SDL_SCALEMODE_PIXELART ) ) {
                    logg::error( "Setting render pixel art scale mode: '{}'",
                                 SDL_GetError() );

                    logg::warning(
                        "Falling back to render nearest scale mode" );

                    if ( !SDL_SetDefaultTextureScaleMode(
                             _applicationState.renderer,
                             SDL_SCALEMODE_NEAREST ) ) {
                        logg::error( "Setting render nearest scale mode: '{}'",
                                     SDL_GetError() );

                        break;
                    }
                }
            }

            // TODO: Set SDL3 logical resolution
            // TODO: Set new SDL3 things

            // Scaling
            {
                const float l_scaleX =
                    ( static_cast< float >(
                          _applicationState.settings.window.width ) /
                      static_cast< float >( _applicationState.logicalWidth ) );
                const float l_scaleY =
                    ( static_cast< float >(
                          _applicationState.settings.window.height ) /
                      static_cast< float >( _applicationState.logicalHeight ) );

                if ( !SDL_SetRenderScale( _applicationState.renderer, l_scaleX,
                                          l_scaleY ) ) {
                    logg::error( "Setting render scale: '{}'", SDL_GetError() );

                    break;
                }
            }

            // Load resources
            if ( !_applicationState.load() ) {
                logg::error( "Loading application state" );

                break;
            }
        }

        // Vsync
        if ( !vsync::init( _applicationState.settings.window.vsync,
                           _applicationState.settings.window.desiredFPS ) ) {
            logg::error( "Initializing Vsync" );

            break;
        }

        // FPS
        FPS::init( _applicationState.totalFramesRendered );

        // Gamepad
        {
            if ( !!( SDL_HasGamepad() ) ) {
                logg::error( "Initializing Gamepad" );

                break;
            }
        }

        logg::debug( "Initialized" );

        l_returnValue = true;
    } while ( false );

    return ( l_returnValue );
}

} // namespace runtime
