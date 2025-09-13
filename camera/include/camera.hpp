#pragma once

#include <SDL3/SDL_rect.h>

#include "stdfloat16.hpp"

using player_t = struct player;

namespace camera {

using camera_t = struct camera {
    camera() = default;
    camera( const camera& ) = default;
    camera( camera&& ) = default;
    ~camera() = default;
    auto operator=( const camera& ) -> camera& = default;
    auto operator=( camera&& ) -> camera& = default;

    auto update( const player_t& _player ) -> bool;

    float16_t zoom = 1;
    float16_t zoomMin = 0.1;
    float16_t zoomMax = 5;
    SDL_FRect rectangle{};
};

} // namespace camera
