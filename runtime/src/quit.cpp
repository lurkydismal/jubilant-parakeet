#include "quit.hpp"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "FPS.hpp"
#include "log.hpp"
#include "vsync.hpp"

namespace runtime {

void quit( applicationState_t& _applicationState ) {
    // Report if SDL error occured before quitting
    {
        std::string_view l_errorMessage = SDL_GetError();

        if ( !l_errorMessage.empty() ) [[unlikely]] {
            logg::error( "Application quit: '{}'", l_errorMessage );
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
            logg::error( "Unloading application state" );
        }

        // Report if SDL error occured during quitting
        {
            std::string_view l_errorMessage = SDL_GetError();

            if ( !l_errorMessage.empty() ) [[unlikely]] {
                logg::error( "Application shutdown: '{}'", l_errorMessage );
            }
        }

        if ( _applicationState->renderer ) [[likely]] {
            SDL_DestroyRenderer( _applicationState->renderer );
        }

        if ( _applicationState.window ) [[likely]] {
            SDL_DestroyWindow( _applicationState.window );
        }
    }

    SDL_Quit();

    logg::debug( "Quitted" );
}

} // namespace runtime
