#!/bin/bash
declare -a partsToBuild=(
    "controls_t"
    "input"
    "FPS"
    "player_t"
    "object_t"
    "camera_t"
    "state_t"
    "animation_t"
    "boxes_t"
    "color_t"
    "settings_t"
    "window_t"
    "vsync"
    "config_t"
    "background_t"
    "HUD_t"
    "asset_t"
    "log"
    "stdfunc"
    "cpp_compatibility"
)
declare -a testsToBuild=(
    "controls_t"
    # "input"
    "FPS"
    "player_t"
    "object_t"
    "camera_t"
    "state_t"
    "animation_t"
    "boxes_t"
    "color_t"
    "settings_t"
    "window_t"
    "vsync"
    # "config_t"
    # "background_t"
    # "HUD_t"
    "asset_t"
    "log"
    "stdfunc"
    # "cpp_compatibility"
)
declare -a staticParts=(
    "inih"
)

export executableMainPackage='main'
export testsMainPackage='test'

if [ ! -z "${DISABLE_OPTIMIZATIONS+x}" ]; then
    BUILD_C_FLAGS_DEBUG+=" -O0"
    BUILD_C_FLAGS_RELEASE+=" -O0"
    BUILD_C_FLAGS_PROFILE+=" -O0"
    BUILD_C_FLAGS_TESTS+=" -O0"
fi
