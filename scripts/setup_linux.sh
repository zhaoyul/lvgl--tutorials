#!/usr/bin/env bash
# setup_linux.sh — Install dependencies and build lvgl_demo on Ubuntu/Debian

set -e

echo "==> Installing build dependencies (requires sudo)..."
sudo apt-get update -qq
sudo apt-get install -y build-essential cmake git libsdl2-dev

echo ""
echo "==> Configuring with CMake..."
cd "$(dirname "$0")/.."
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo ""
echo "==> Building..."
cmake --build build -j"$(nproc)"

echo ""
echo "==> Done!  Run the demo with:"
echo "    ./build/bin/lvgl_demo"
