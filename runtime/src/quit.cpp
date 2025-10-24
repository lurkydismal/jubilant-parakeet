#include "quit.hpp"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "FPS.hpp"
#include "log.hpp"
#include "slickdl/error.hpp"
#include "vsync.hpp"

namespace runtime {

void quit( applicationState_t& _applicationState ) {
    // Report if SDL error occured before quitting
    {
        const std::optional l_errorMessage = slickdl::error::get();

        if ( l_errorMessage ) [[unlikely]] {
            logg$error( "Application quit: '{}'", l_errorMessage.value() );
        }
    }

    // FPS
    FPS::quit();

    // Vsync
    vsync::quit();

    // Asset loader
#if 0
    asset::quit();
#endif

    // Application state
    {
        if ( !_applicationState.unload() ) [[unlikely]] {
            logg$error( "Unloading application state" );
        }

        // Report if SDL error occured during quitting
        {
            const std::optional l_errorMessage = slickdl::error::get();

            if ( l_errorMessage ) [[unlikely]] {
                logg$error( "Application shutdown: '{}'",
                            l_errorMessage.value() );
            }
        }

        if ( _applicationState.renderContext.renderer ) [[likely]] {
            SDL_DestroyRenderer( _applicationState.renderContext.renderer );
        }

        if ( _applicationState.renderContext.window.handle ) [[likely]] {
            SDL_DestroyWindow( _applicationState.renderContext.window.handle );
        }
    }

    SDL_Quit();

    logg$debug( "Quitted" );
}

} // namespace runtime
