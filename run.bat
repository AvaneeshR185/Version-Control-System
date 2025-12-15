@echo off
set SRC=main.cpp
set OUT=main.exe

echo Compiling %SRC% ...
g++ -std=c++17 -O2 %SRC% -o %OUT%

if %ERRORLEVEL% neq 0 (
    echo Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

echo Compilation successful! Running program...
echo ----------------------------------------
%OUT%
pause
