#!/bin/bash
i=0

while IFS= read -r line; do
    if (( i % 2 )); then
        printf '\e[48;5;236m''%s''\e[0m\n' "$line"

    else
        printf '%s\n' "$line"
    fi

    (( i++ ))
done
