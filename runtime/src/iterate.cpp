#include "iterate.hpp"

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
                _applicationState.renderContext.renderer.clear();
            }

            // TODO: Scene

            // End frame
            {
                _applicationState.renderContext.renderer.present();
            }
        }

        // TODO: Logic

        l_returnValue = true;
    }

    return ( l_returnValue );
}

} // namespace runtime
