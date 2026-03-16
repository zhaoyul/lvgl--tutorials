#!/usr/bin/env bash
# setup_macos.sh — Install dependencies and build lvgl_demo on macOS

set -e

# Check for Homebrew
if ! command -v brew &>/dev/null; then
    echo "==> Homebrew not found.  Installing..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

echo "==> Installing cmake and sdl2 via Homebrew..."
brew install cmake sdl2

echo ""
echo "==> Configuring with CMake..."
cd "$(dirname "$0")/.."
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo ""
echo "==> Building..."
cmake --build build -j"$(sysctl -n hw.ncpu)"

echo ""
echo "==> Done!  Run the demo with:"
echo "    ./build/bin/lvgl_demo"
