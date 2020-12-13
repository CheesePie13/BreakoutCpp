@ECHO OFF 

:: Get version
set /p version=<version.txt

:: Clear or create bin\win-release
if exist bin\win-release rmdir /S /Q bin\win-release
mkdir bin\win-release

:: Compile with mingw64
g++ -o bin\win-release\BreakoutCppWin_%version%.exe src\*.cpp third-party\src\*.c -DWINDOWS -DVERSION=\"%version%\" -Ithird-party\include -Lthird-party\lib-win -lglfw3 -lgdi32 -Wall -O2
if %errorlevel% neq 0 exit /b %errorlevel%

:: Copy shaders to bin\win-release
robocopy assets\shaders bin\win-release\shaders > nul

exit 0