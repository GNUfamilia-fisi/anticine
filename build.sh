#/bin/sh

set -ex

ncurses_dir="/usr/include/ncurses.h"
build_dir="bin"

if ! ( ls $ncurses_dir 1>/dev/null 2>&1 ); then
    echo "fatal: ncurses.h not found"
    exit 1
fi

# Build

if [ ! -d $build_dir ]; then
    mkdir $build_dir
fi

g++ src/main.cpp -o $build_dir/main -lncursesw -Iinclude
