#include "iterate.hpp"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace runtime {

auto iterate( applicationState_t& _applicationState ) -> bool {
    bool l_returnValue = false;

    // TODO: Hot reload

    {
        // TODO: Camera

        // Render
        {
            // Begin frame
            {
                SDL_RenderClear( _applicationState.renderContext.renderer );
            }

            // TODO: Scene

            // End frame
            {
                SDL_RenderPresent( _applicationState.renderContext.renderer );
            }
        }

        // TODO: Logic

        l_returnValue = true;
    }

    return ( l_returnValue );
}

} // namespace runtime
