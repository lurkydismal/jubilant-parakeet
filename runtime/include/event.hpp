#pragma once

#include <SDL3/SDL_events.h>

#include "runtime.hpp"

namespace runtime {

using event_t = SDL_Event;

auto event( applicationState_t& _applicationState, const event_t& _event )
    -> bool;

} // namespace runtime
