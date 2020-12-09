@ECHO OFF 
if exist win-release rmdir /S /Q win-release
mkdir win-release
g++ -o win-release/BreakoutCppWinRelease.exe src/*.cpp -Iinclude -Llib -lglfw3 -lgdi32 -O2