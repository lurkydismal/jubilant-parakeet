#pragma once

#include <cstdint>

#include "stdfloat16.hpp"

// Software vsync implementation
namespace vsync {

using vsync_t = enum class vsync : uint8_t {
    off,
    software,
};

void init( vsync_t _vsyncType, float16_t _desiredFPS );
void quit();

void begin();
void end();

} // namespace vsync
