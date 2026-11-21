@echo off
setlocal
cd /d "%~dp0"

if not defined CXX if exist "E:\mingw32\bin\g++.exe" set "CXX=E:\mingw32\bin\g++.exe"
if not defined CXX set "CXX=g++"

rem Prefer the x64 SFML build that matches the 64-bit MinGW compiler
if not defined SFML_DIR if exist "%~dp0docs\SFML-2.6.2-x64\SFML-2.6.2\lib\libsfml-graphics.a" set "SFML_DIR=%~dp0docs\SFML-2.6.2-x64\SFML-2.6.2"
if not defined SFML_DIR if exist "E:\SFML-2.6.2\lib\libsfml-graphics.a" set "SFML_DIR=E:\SFML-2.6.2"
if not defined SFML_DIR if exist "%~dp0docs\SFML-2.6.2\lib\libsfml-graphics.a" set "SFML_DIR=%~dp0docs\SFML-2.6.2"
if not defined SFML_DIR if exist "C:\SFML-2.6.2\lib\libsfml-graphics.a" set "SFML_DIR=C:\SFML-2.6.2"
if not defined SFML_DIR set "SFML_DIR=%~dp0docs\SFML-2.6.2-x64\SFML-2.6.2"

if /i not "%CXX%"=="g++" for %%I in ("%CXX%") do set "PATH=%%~dpI;%PATH%"
set "PATH=%SFML_DIR%\bin;%PATH%"

call "%~dp0build.bat"
if errorlevel 1 (
    exit /b 1
)

echo Starting Vault Heist: The Living Vault...
start "" "main.exe"
exit /b 0
