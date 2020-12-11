@ECHO OFF 

:: Clear or create bin\win-debug
if exist bin\win-debug rmdir /S /Q bin\win-debug
mkdir bin\win-debug

:: Compile with mingw64
g++ -o bin\win-debug\BreakoutCppWinDebug.exe src\*.cpp src\*.c -Iinclude -Llib -lglfw3 -lgdi32 -Wall -O0 -g
if %errorlevel% neq 0 exit /b %errorlevel%

:: Copy shaders to bin\win-debug
robocopy src\shaders bin\win-debug\shaders > nul

exit 0