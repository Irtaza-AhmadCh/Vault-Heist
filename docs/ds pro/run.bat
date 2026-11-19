@echo off
if not exist main.exe (
    echo main.exe not found, building first...
    call build.bat
)
echo Launching SFML application...
start "" main.exe

