#!/bin/bash

dir=$(pwd)
echo "Building $dir"
# rm -rf build
mkdir -p build
cmake -B build -S .
cmake -B build -S .
cmake --build build
cmake --install build --prefix $dir -v