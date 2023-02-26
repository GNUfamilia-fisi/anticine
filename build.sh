#!/bin/bash

set -ex

build_dir="bin"
includes="./include"
bin_name="anticine"
cpp_flags="-Wall -Wextra -pedantic"

# Build
if [ ! -d $build_dir ]; then
    mkdir $build_dir
fi

g++ src/main.cpp -o $build_dir/$bin_name \
    $(for i in $includes; do echo -n "-I$i "; done) \
    $cpp_flags
