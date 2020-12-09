@ECHO OFF 
if exist win-debug rmdir /S /Q win-debug
mkdir win-debug
g++ -o win-debug/BreakoutCppWinDebug.exe src/*.cpp -Iinclude -Llib -lglfw3 -lgdi32 -Wall -O0 -g