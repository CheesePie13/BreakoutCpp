#!/bin/bash

# Clear or create bin/mac-release
rm -r bin/mac-release
mkdir -p bin/mac-release

# Compile @todo: compile .c files with gcc
g++ -o bin/mac-release/BreakoutCppMacrelease src/*.cpp src/*.c -DMACOS -Iinclude -Llib-mac -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -Wall -O0 -g

# Copy shaders to bin/mac-release
cp -r assets/shaders bin/mac-release/shaders
