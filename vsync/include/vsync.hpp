#pragma once

#include <cstdint>

#include "stdfloat16.hpp"

// Software vsync implementation
namespace vsync {

enum class vsync_t : uint8_t {
    off,
    unknownVsync,
};

auto init( const vsync_t _vsyncType, const float16_t _desiredFPS ) -> bool;
void quit();

void begin();
void end();

} // namespace vsync
