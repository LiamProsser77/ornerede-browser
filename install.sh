#!/bin/bash

set -e

echo "Installing Ornerede Browser..."

if ! command -v cmake >/dev/null 2>&1; then
    echo "Error: CMake is not installed."
    exit 1
fi

if ! command -v c++ >/dev/null 2>&1; then
    echo "Error: C++ compiler is not installed."
    exit 1
fi

cmake -S . -B build
cmake --build build

echo "Ornerede has been built successfully."
echo "Run it with: ./build/ornerede"
