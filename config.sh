#!/bin/bash
declare -a partsToBuild=(
    "runtime"
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
    "character_t"
    "asset_t"
    "watch_t"
    "log"
    "stdfunc"
)
declare -a testsToBuild=(
    # "runtime"
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
    # "config_t"
    # "background_t"
    # "HUD_t"
    # "character_t"
    "asset_t"
    # "watch_t"
    "log"
    "stdfunc"
)
declare -a staticParts=(
    "inih"
    "plthook"
)
declare -a rootSharedObjectName="root"

export executableMainPackage='main'
export testsMainPackage='test'
