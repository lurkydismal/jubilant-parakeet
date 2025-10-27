#include "quit.hpp"

#include "FPS.hpp"
#include "log.hpp"
#include "slickdl/error.hpp"
#include "slickdl/init_quit.hpp"
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
    }

    slickdl::quit::all();

    logg$debug( "Quitted" );
}

} // namespace runtime
