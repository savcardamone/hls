#!/bin/bash
## Convenience script to nuke build/ and rebuild/ test the project.

# Make sure we run everything in the root directory of the project
cd "$(dirname "${BASH_SOURCE[0]}")"

# Clean up any existing build and remake everything
rm -rf build/*
cmake \
    -DCMAKE_C_COMPILER="/usr/local/bin/clang" -DCMAKE_CXX_COMPILER="/usr/local/bin/clang++" \
    -S . -B build
cmake --build build
cd build/test && ctest --verbose
