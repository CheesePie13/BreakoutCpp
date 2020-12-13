#!/bin/bash

# Get version
version=$(cat version.txt)

# Clear or create bin/mac-debug
rm -r bin/mac-debug
mkdir -p bin/mac-debug

# Compile @todo: compile .c files with gcc
g++ -o bin/mac-debug/BreakoutCppMac_debug.app src/*.cpp src/third-party/*.c -DMACOS -DVERSION=\"$version-debug\" -Iinclude -Llib-mac -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -Wall -O0 -g

# Copy shaders to bin/mac-debug
cp -r assets/shaders bin/mac-debug/shaders
