#!/bin/bash
export SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export BUILD_DIRECTORY_NAME='out'
export BUILD_DIRECTORY="$SCRIPT_DIRECTORY/$BUILD_DIRECTORY_NAME"
export LINK_FLAGS="-lSDL3 -s -O1"
export LINKER="ccache gcc"
export EXECUTABLE_NAME="main.out"

clear

source './config.sh' && {

mkdir -p "$BUILD_DIRECTORY"

for partToBuild in "${partsToBuild[@]}"; do

    source "$partToBuild/config.sh" && './build_general.sh' "$partToBuild"

    if [ $? -ne 0 ]; then
        break
    fi

done

if [ $? -eq 0 ]; then
    printf -v partsToBuildAsString "$BUILD_DIRECTORY/lib%s.a " "${partsToBuild[@]}"

    echo $partsToBuildAsString

    $LINKER $LINK_FLAGS $partsToBuildAsString -o $BUILD_DIRECTORY/$EXECUTABLE_NAME
fi

}
