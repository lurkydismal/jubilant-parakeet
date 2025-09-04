#!/bin/bash
TARGET_DIRECTORY=$1

INCLUDE_PATHS=$(echo $FILES_TO_INCLUDE | sed "s~[^ ]*~-I $TARGET_DIRECTORY/&~g")
COMPILE_PATHS=$(echo $FILES_TO_COMPILE | sed "s~[^ ]*~$TARGET_DIRECTORY/&~g")

needBuild=0

# Check if build is needed
{
    cd "$TARGET_DIRECTORY" || exit

    # TODO: Better name
    newest_file=$(fd -e c -e cpp -e h -e hpp . | xargs stat --format '%Y %n' | sort -n | tail -1 | cut -d' ' -f2-)

    if [[ -f "$newest_file" && -f "$BUILD_DIRECTORY/$OUTPUT_FILE" ]] && [[ "$newest_file" -nt "$BUILD_DIRECTORY/$OUTPUT_FILE" ]]; then
        needBuild=1
    fi

    cd - >'/dev/null' || exit
}

if ((needBuild)); then
    source "$SCRIPT_DIRECTORY/config.sh" &&
        make clean &&
        make \
            "BUILD_FLAGS=$2" \
            "DEFINES=$3" \
            "INCLUDES=$4" \
            "FILES_TO_INCLUDE=$INCLUDE_PATHS" \
            "FILES_TO_COMPILE=$COMPILE_PATHS" &&
        mv "$OUTPUT_FILE" "$BUILD_DIRECTORY" &&
        cd $TARGET_DIRECTORY &&
        clang-format --style="file:$SCRIPT_DIRECTORY/.clang-format" \
            -i \
            $(echo $FILES_TO_INCLUDE $FILES_TO_COMPILE) &&
        cd "$SCRIPT_DIRECTORY" || exit
fi
