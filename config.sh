#!/bin/bash
export partsToBuild=(
    "runtime"
    "control"
    "input"
    "FPS"
    "object"
    "camera"
    "state"
    "animation"
    "boxes"
    "window"
    "vsync"
    # "font_t"
    "asset"
    "watch"
)
export testsToBuild=(
    # "runtime"
    # "control"
    # "input"
    "FPS"
    # "object"
    # "camera"
    # "state"
    "animation"
    "boxes"
    # "window"
    "vsync"
    # "font_t"
    # "asset"
    # "watch"

    # "plthook"
    "ctre"

    # "slickdl"
    "arhodigp"
    "log"
    "forward_circular_buffer"
    "color"
    "stdfunc"

    "test"
)
export staticParts=(
    "plthook"
    "ctre"

    "slickdl"
    "arhodigp"
    "log"
    "forward_circular_buffer"
    "color"
    "stdfunc"
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

BUILD_DEFINES_RELEASE+=(
    "NDEBUG"
)

BUILD_DEFINES_PROFILE+=(
    "NDEBUG"
)

BUILD_INCLUDES+=()

LIBRARIES_TO_LINK+=(
    "mimalloc"
    "zstd"
    "elf"
    "unwind"
)

LIBRARIES_TO_LINK_TESTS+=()

EXTERNAL_LIBRARIES_TO_LINK+=(
    "snappy"
    "sdl3"
    "sdl3-image"
)

EXTERNAL_LIBRARIES_TO_LINK_TESTS+=(
    "gtest"
    "gmock"
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
