#!/bin/bash

set -e

echo "Installing Ornerede build requirements..."

if command -v apt >/dev/null 2>&1; then
    sudo apt update
    sudo apt install -y build-essential cmake qt6-base-dev

elif command -v dnf >/dev/null 2>&1; then
    sudo dnf install -y gcc-c++ cmake qt6-qtbase-devel

elif command -v pacman >/dev/null 2>&1; then
    sudo pacman -S --needed base-devel cmake qt6-base

else
    echo "Unsupported package manager."
    exit 1
fi

echo "Building Ornerede..."

cmake -S . -B build
cmake --build build

echo "Build complete!"
echo "Executable: build/ornerede"
