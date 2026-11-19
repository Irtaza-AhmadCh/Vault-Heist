@echo off
if not exist main.exe (
    call build.bat
)
if exist main.exe (
    echo Starting Vault Heist: The Living Vault...
    start main.exe
)
