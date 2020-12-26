#!/bin/bash
set -e

# Clear or create bin/mac-debug
rm -r bin/mac-debug
mkdir -p bin/mac-debug

# Compile tests
g++ -o bin/mac-debug/BreakoutCppMac_tests.app tests/*.cpp -DMACOS -Wall -O0 -g

# Run tests
./bin/mac-debug/BreakoutCppMac_tests.app

# Get version
version=$(cat version.txt)

# Compile @todo: compile .c files with gcc
g++ -o bin/mac-debug/BreakoutCppMac_debug.app src/*.cpp third-party/src/*.c -DMACOS -DVERSION=\"$version-debug\" -Ithird-party/include -Lthird-party/lib-mac -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -Wall -O0 -g

# Copy shaders to bin/mac-debug
cp -r assets/shaders bin/mac-debug/shaders
