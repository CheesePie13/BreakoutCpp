@ECHO OFF 
if exist win-release rmdir /S /Q win-release
mkdir win-release
g++ -o win-release/BreakoutCppWinRelease.exe -O2 src/main.cpp