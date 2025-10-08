#pragma once

#include <cstdint>
#include <type_traits>

#include "stdfloat16.hpp"

namespace vsync {

using vsync_t = enum class vsync : uint8_t {
    off,
    software,
};

using vsyncUnderlying_t = std::underlying_type_t< vsync_t >;

void init( vsync_t _vsyncType, float16_t _desiredFPS );
void quit();

void begin();
void end();

} // namespace vsync
