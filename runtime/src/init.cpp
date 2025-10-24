#include "init.hpp"

#include <SDL3/SDL_video.h>

#include <format>

#include "FPS.hpp"
#include "log.hpp"
#include "slickdl/render_texture.hpp"
#include "slickdl/surface.hpp"
#include "vsync.hpp"
#include "window.hpp"

namespace runtime {

auto init( applicationState_t& _applicationState ) -> bool {
    bool l_returnValue = false;

    do {
        // Default scale mode
        _applicationState.renderContext.renderer.defaultTextureScaleMode(
            slickdl::scale_t::pixelArt );

#if 0
        {
                logg$error( "Setting render pixel art scale mode: '{}'",
                            slickdl::error::get().value() );

                logg::warning( "Falling back to render nearest scale mode" );

            _applicationState.renderContext.renderer.defaultTextureScaleMode(slickdl::scale_t::nearest);
                    logg$error( "Setting render nearest scale mode: '{}'",
                                slickdl::error::get().value() );
        }
#endif

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

            _applicationState.renderContext.renderer.scale( l_scaleX,
                                                            l_scaleY );

#if 0
            logg$error( "Setting render scale: '{}'",
                        slickdl::error::get().value() );
#endif
        }

        // Load resources
        if ( !_applicationState.load() ) {
            logg$error( "Loading application state" );

            break;
        }

        // Vsync
        vsync::init( _applicationState.renderContext.window.vsync,
                     _applicationState.renderContext.window.desiredFPS );

        // FPS
        FPS::init( _applicationState.renderContext.window.totalFramesRendered );

        // Gamepad
        {
            if ( SDL_HasGamepad() ) {
                logg$error( "Initializing Gamepad" );

                break;
            }
        }

        l_returnValue = true;
    } while ( false );

    logg$debug( "Initialized" );

    return ( l_returnValue );
}

} // namespace runtime
