@echo off
echo Building SFML project...
"E:\mingw32\bin\g++.exe" -std=c++17 -Wall -Wextra src/main.cpp -o main.exe -I"E:\SFML-2.6.2\include" -L"E:\SFML-2.6.2\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -static-libgcc -static-libstdc++
if %ERRORLEVEL% equ 0 (
    echo Build successful! Run with run.bat or .\main.exe
) else (
    echo Build failed.
)

