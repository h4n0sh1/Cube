@echo off
REM Build script for the 3D Formula project
REM Pure C - No dependencies!

set GCC=C:\Users\h4wk\Downloads\w64devkit\bin\gcc.exe

%GCC% main.c -o cube.exe -O2 -Wall
if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Running...
    cube.exe
) else (
    echo Build failed!
)
