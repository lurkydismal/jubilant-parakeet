#!/bin/bash
set -e

SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

ASAN_SYMBOLIZER_PATH=
UBSAN_SYMBOLIZER_PATH= 
ASAN_OPTIONS='verbosity=2'
LSAN_OPTIONS='suppressions=./lsan.suppress:verbosity=2'

clear

./out/main.out 2>&1 | stdbuf -oL awk -v path="$SCRIPT_DIRECTORY/" '
/\(BuildId:/ {
match( $0, /\+0x([0-9a-fA-F]+)/, matches )

cmd = "llvm-symbolizer -e ./out/main.out <<< " "0x"matches[ 1 ]

sub( path, "", $0 )

sub( /\ \(BuildId:[^)]*\)/, "", $0 )

first = 0

while ( cmd | getline symbolized ) {
    if ( length( symbolized ) ) {
        sub( path, "", symbolized )

        if (first == 0) {
            print $0 "  ==>  " symbolized
        } else {
            printf "%*s  ==>  %s\n", length( $0 ), "", symbolized
        }

        first = 1
    }
}

close( cmd )

next
}
{
    sub( path, "", symbolized )

    print $0
}
'
