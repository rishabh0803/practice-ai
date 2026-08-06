
@echo off
setlocal enabledelayedexpansion

REM Stop on error (approximation of set -e)
if errorlevel 1 exit /b %errorlevel%

REM Remove object files
del /q *.o 2>nul

REM Remove old library
del /q ..\lib\libmlcpp.a 2>nul

REM Compile all .cpp files into .o
g++ -fopenmp -O3 -I ..\include -c *.cpp
if errorlevel 1 exit /b %errorlevel%

REM Collect all .o files into a variable
set OBJFILES=
for %%f in (*.o) do (
    set OBJFILES=!OBJFILES! %%f
)

REM Create static library
ar rcs ..\lib\libmlcpp.a !OBJFILES!

REM Clean object files again
del /q *.o 2>nul

echo ..\lib\libmlcpp.a created

