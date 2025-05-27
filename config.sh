#!/bin/bash
declare -a partsToBuild=(
    "FPS"
    "object_t"
    "state_t"
    "boxes_t"
    "animation_t"
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
    "object_t"
    "state_t"
    "boxes_t"
    "animation_t"
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
