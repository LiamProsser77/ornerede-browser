#!/bin/bash

set -e

echo "================================"
echo "       Ornerede Browser"
echo "================================"

echo "Checking dependencies..."

if ! command -v cmake >/dev/null 2>&1; then
    echo "CMake is required."
    exit 1
fi

if ! command -v c++ >/dev/null 2>&1; then
    echo "A C++ compiler is required."
    exit 1
fi

echo "Configuring project..."

cmake -S . -B build

echo "Building Ornerede..."

cmake --build build --parallel

echo "================================"
echo "Build completed!"
echo "================================"

echo "Run Ornerede with:"
echo "./build/ornerede"
