#!/bin/bash
export partsToBuild=(
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
    # "font_t"
    "asset_t"
    "watch_t"
    "log"
    "stdfunc"
)
export testsToBuild=(
    # "runtime"
    "controls_t"
    "input"
    "FPS"
    # "player_t"
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
    # "font_t"
    "asset_t"
    # "watch_t"
    "log"
    "stdfunc"
)
export staticParts=(
    "inih"
    "plthook"
)
export rootSharedObjectName="root"

export executableMainPackage='main'
export testsMainPackage='test'

BUILD_DEFINES+=(
    "INI_ALLOW_INLINE_COMMENTS=1"
    "INI_STOP_ON_FIRST_ERROR=1"
    "INI_CALL_HANDLER_ON_NEW_SECTION=1"
)

BUILD_INCLUDES+=(
    "runtime/applicationState_t/include"
)

LIBRARIES_TO_LINK+=(
    "mimalloc"
    "elf"
    "unwind"
)

EXTERNAL_LIBRARIES_TO_LINK+=(
    "snappy"
    "sdl3"
    "sdl3-image"
)

export MODULE_NAME_FIELD_WIDTH=14
