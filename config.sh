#!/bin/bash
export partsToBuild=(
    "runtime"
    "control"
    "input"
    "FPS"
    "object_t"
    "camera"
    "state_t"
    "animation_t"
    "boxes_t"
    "window"
    "vsync"
    # "font_t"
    "asset"
    "watch_t"
    "log"
    "stdfunc"
)
export testsToBuild=(
    # "runtime"
    "control"
    "input"
    "FPS"
    "object_t"
    # "camera"
    "state_t"
    "animation_t"
    "boxes_t"
    "window"
    "vsync"
    # "font_t"
    "asset"
    # "watch_t"
    "log"
    "stdfunc"
)
export staticParts=(
    "plthook"
    "slickdl"
    "forward_circular_buffer"
)
export rootSharedObjectName="root"

export executableMainPackage='main'
export testsMainPackage='test'

BUILD_DEFINES+=(
    # xxhash
    "XXH_CPU_LITTLE_ENDIAN=1"
    "XXH_INLINE_ALL"
    "XXH_NO_STDLIB"
    "XXH_NO_STREAM"
    "XXH_STATIC_LINKING_ONLY"
)

BUILD_INCLUDES+=()

LIBRARIES_TO_LINK+=(
    "mimalloc"
    "zstd"
    "elf"
    "unwind"
)

EXTERNAL_LIBRARIES_TO_LINK+=(
    "snappy"
    "sdl3"
    "sdl3-image"
)

export MODULE_NAME_FIELD_WIDTH=24

export FAIL_COLOR="$RED_LIGHT_COLOR"
export BUILD_TYPE_COLOR="$PURPLE_LIGHT_COLOR"
export DEFINES_COLOR="$CYAN_LIGHT_COLOR"
export INCLUDES_COLOR="$BLUE_LIGHT_COLOR"
export LIBRARIES_COLOR="$BLUE_LIGHT_COLOR"
export EXTERNAL_LIBRARIES_COLOR="$BLUE_LIGHT_COLOR"
export PARTS_TO_BUILD_COLOR="$YELLOW_COLOR"
export SKIPPING_PART_IN_BUILD_COLOR="$GREEN_LIGHT_COLOR"
export BUILT_EXECUTABLE_COLOR="$GREEN_LIGHT_COLOR"
export SECTIONS_TO_STRIP_COLOR="$RED_LIGHT_COLOR"
