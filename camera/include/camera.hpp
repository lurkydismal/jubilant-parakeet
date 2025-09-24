#pragma once

#include <glm/glm.hpp>

#include "stdfloat16.hpp"

namespace camera {

using camera_t = struct camera {
    camera() = default;
    camera( const camera& ) = default;
    camera( camera&& ) = default;
    ~camera() = default;
    auto operator=( const camera& ) -> camera& = default;
    auto operator=( camera&& ) -> camera& = default;

    void update() {}

    float16_t zoom = 1;
    float16_t zoomMin = 0.1;
    float16_t zoomMax = 5;
    glm::vec2 position{};
};

} // namespace camera
