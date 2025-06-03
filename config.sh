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
    "config_t"
    "settings_t"
    "window_t"
    "vsync"
    "asset_t"
    "log"
    "stdfunc"
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
    "asset_t"
    "log"
    "stdfunc"
)
declare -a staticParts=(
)

export executableMainPackage='main'
export testsMainPackage='test'
