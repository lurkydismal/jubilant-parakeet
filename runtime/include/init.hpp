#pragma once

#include <SDL3/SDL_init.h>

#include <span>

#include "FPS.hpp"
#include "log.hpp"
#include "runtime.hpp"
#include "vsync.hpp"

namespace runtime {

auto init( applicationState_t& _applicationState,
           [[maybe_unused]] std::span< std::string_view > _arguments ) -> bool {
    bool l_returnValue = false;

    do {
        // Generate application state
        {
            // Metadata
            {
                logg::info(
                    "Window name: '{}', Version: '{}', Identifier: '{}'",
                    window::window_t::g_name,
                    applicationState_t::metadata::g_version,
                    applicationState_t::metadata::g_identifier );

                if ( !SDL_SetAppMetadata(
                         std::string( window::window_t::g_name ).c_str(),
                         std::format( "{}",
                                      applicationState_t::metadata::g_version )
                             .c_str(),
                         std::string(
                             applicationState_t::metadata::g_identifier )
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
                         std::string( window::window_t::g_name ).c_str(),
                         _applicationState.renderContext.window.width,
                         _applicationState.renderContext.window.height,
                         ( SDL_WINDOW_INPUT_FOCUS ),
                         &( _applicationState.renderContext.window.handle ),
                         &( _applicationState.renderContext.renderer ) ) ) {
                    logg::error( "Window or Renderer creation: '{}'",
                                 SDL_GetError() );

                    break;
                }

                logg::variable( _applicationState.renderContext.window.width );
                logg::variable( _applicationState.renderContext.window.height );
            }

            // Default scale mode
            {
                if ( !SDL_SetDefaultTextureScaleMode(
                         _applicationState.renderContext.renderer,
                         SDL_SCALEMODE_PIXELART ) ) {
                    logg::error( "Setting render pixel art scale mode: '{}'",
                                 SDL_GetError() );

                    logg::warning(
                        "Falling back to render nearest scale mode" );

                    if ( !SDL_SetDefaultTextureScaleMode(
                             _applicationState.renderContext.renderer,
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
                          _applicationState.renderContext.window.width ) /
                      static_cast< float >(
                          _applicationState.renderContext.logicalWidth ) );
                const float l_scaleY =
                    ( static_cast< float >(
                          _applicationState.renderContext.window.height ) /
                      static_cast< float >(
                          _applicationState.renderContext.logicalHeight ) );

                if ( !SDL_SetRenderScale(
                         _applicationState.renderContext.renderer, l_scaleX,
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
        if ( !vsync::init(
                 _applicationState.renderContext.window.vsync,
                 _applicationState.renderContext.window.desiredFPS ) ) {
            logg::error( "Initializing Vsync" );

            break;
        }

        // FPS
        FPS::init( _applicationState.renderContext.totalFramesRendered );

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
