@ECHO OFF 
if exist win-debug rmdir /S /Q win-debug
mkdir win-debug
g++ -o win-debug/BreakoutCppWinDebug.exe src/main.cpp -Wall -O0 -g