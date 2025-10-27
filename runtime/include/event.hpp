#pragma once

#include "runtime.hpp"

namespace runtime {

using event_t = slickdl::events::event_t;

auto event( applicationState_t& _applicationState, const event_t& _event )
    -> bool;

} // namespace runtime
