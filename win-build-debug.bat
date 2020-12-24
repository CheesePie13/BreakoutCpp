@ECHO OFF

:: Clear or create bin\win-debug
if exist bin\win-debug rmdir /S /Q bin\win-debug
mkdir bin\win-debug
if %errorlevel% neq 0 exit /b %errorlevel%

:: Compile tests with mingw64
g++ -o bin\win-debug\BreakoutCppWin_tests.exe tests\*.cpp -DWINDOWS -Wall -O0 -g
if %errorlevel% neq 0 exit /b %errorlevel%

:: Run tests
bin\win-debug\BreakoutCppWin_tests.exe
if %errorlevel% neq 0 exit /b %errorlevel%

:: Get version
set /p version=<version.txt

:: Compile with mingw64
g++ -o bin\win-debug\BreakoutCppWin_debug.exe src\*.cpp third-party\src\*.c -DWINDOWS -DVERSION=\"%version%-debug\" -Ithird-party\include -Lthird-party\lib-win -lglfw3 -lgdi32 -Wall -O0 -g
if %errorlevel% neq 0 exit /b %errorlevel%

:: Copy shaders to bin\win-debug
robocopy assets\shaders bin\win-debug\shaders > nul

exit 0