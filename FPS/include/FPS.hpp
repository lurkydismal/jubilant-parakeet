#pragma once

#include <atomic>
#include <cstddef>

namespace FPS {

void init( std::atomic< size_t >& _frameCount );
void quit();

} // namespace FPS
