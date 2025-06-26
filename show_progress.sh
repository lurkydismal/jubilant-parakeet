#!/bin/bash
trap 'COLUMNS=$(tput cols)' WINCH

# Unicode block characters for fine-grained bar filling
BAR_CHARS=( " " "▏" "▎" "▍" "▌" "▋" "▊" "▉" "█" )
BAR_DIV=$(( ${#BAR_CHARS[@]} - 1 ))

export total=1
counter=-1

# TODO: Fix
show_progress() {
    return

    if (( counter >= total )); then
        return
    fi

    ((counter++))

    local cols=${COLUMNS:-$(tput cols)}
    # Reserve space for percent etc
    local bar_width=$(( cols - 10 ))
    bar_width=$(( bar_width / 2 ))
    (( bar_width < 10 )) && bar_width=10

    local total_units=$(( bar_width * BAR_DIV ))
    local done_units=$(( counter * total_units / total ))
    (( done_units > total_units )) && done_units=total_units

    local percent_int=$(( counter * 100 / total ))
    local percent_frac=$(( (counter * 10000 / total) % 100 ))

    local full_chars=$(( done_units / BAR_DIV ))
    local partial_index=$(( done_units % BAR_DIV ))
    local rem_chars=$(( bar_width - full_chars - (partial_index > 0 ? 1 : 0) ))

    local bar=""
    for ((i=0; i<full_chars; i++)); do
        bar+="${BAR_CHARS[BAR_DIV]}"
    done

    if (( partial_index > 0 )); then
        bar+="${BAR_CHARS[partial_index]}"
    fi

    for ((i=0; i<rem_chars; i++)); do
        bar+="${BAR_CHARS[0]}"
    done

    # Save current cursor position
    tput sc

    # Move cursor to bottom line
    bottom=$(( $(tput lines) - 1 ))
    tput cup "$bottom" 0

    # Clear that full line
    tput el

    # Draw your bar
    printf "[%s] %3d.%02d%%" "$bar" "$percent_int" "$percent_frac"

    # Restore cursor
    tput rc
}
