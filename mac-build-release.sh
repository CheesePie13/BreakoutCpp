#!/bin/bash

# Get version
version=$(cat version.txt)

# Clear or create bin/mac-release
rm -r bin/mac-release
mkdir -p bin/mac-release

# Compile @todo: compile .c files with gcc
g++ -o bin/mac-release/BreakoutCppMac_$version.app src/*.cpp third-party/src/*.c -DMACOS -DVERSION=\"$version\" -Ithird-party/include -Lthird-party/lib-mac -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -Wall -O2

# Copy shaders to bin/mac-release
cp -r assets/shaders bin/mac-release/shaders
