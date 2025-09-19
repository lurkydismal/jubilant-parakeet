#pragma once

#include "runtime.hpp"

namespace runtime {

auto init( applicationState_t& _applicationState,
           std::span< std::string_view > _arguments ) -> bool;

} // namespace runtime
