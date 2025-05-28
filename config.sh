#!/bin/bash
declare -a partsToBuild=(
    "FPS"
    "camera_t"
    "object_t"
    "state_t"
    "animation_t"
    "boxes_t"
    "color_t"
    "settings_t"
    "window_t"
    "vsync"
    "asset_t"
    "log"
    "stdfunc"
)
declare -a testsToBuild=(
    "FPS"
    # "camera_t"
    "object_t"
    "state_t"
    "animation_t"
    "boxes_t"
    "color_t"
    "settings_t"
    "window_t"
    "vsync"
    "asset_t"
    "log"
    "stdfunc"
)
declare -a staticParts=(
)

export executableMainPackage='main'
export testsMainPackage='test'
