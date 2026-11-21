@echo off
setlocal
pushd "%~dp0"

echo Building Vault Heist: The Living Vault (Game Codebase)...

if not defined CXX if exist "E:\mingw32\bin\g++.exe" set "CXX=E:\mingw32\bin\g++.exe"
if not defined CXX set "CXX=g++"

if /i "%CXX%"=="g++" (
    where g++ >nul 2>nul
    if errorlevel 1 (
        echo [ERROR] g++ was not found on PATH.
        echo Set CXX to a MinGW compiler executable or add its bin folder to PATH.
        goto :failed
    )
) else if not exist "%CXX%" (
    echo [ERROR] Compiler was not found at "%CXX%".
    goto :failed
)

rem Prefer the x64 SFML build that matches the 64-bit MinGW compiler
if not defined SFML_DIR if exist "%~dp0docs\SFML-2.6.2-x64\SFML-2.6.2\lib\libsfml-graphics.a" set "SFML_DIR=%~dp0docs\SFML-2.6.2-x64\SFML-2.6.2"
if not defined SFML_DIR if exist "E:\SFML-2.6.2\lib\libsfml-graphics.a" set "SFML_DIR=E:\SFML-2.6.2"
if not defined SFML_DIR if exist "%~dp0docs\SFML-2.6.2\lib\libsfml-graphics.a" set "SFML_DIR=%~dp0docs\SFML-2.6.2"
if not defined SFML_DIR if exist "C:\SFML-2.6.2\lib\libsfml-graphics.a" set "SFML_DIR=C:\SFML-2.6.2"
if not defined SFML_DIR set "SFML_DIR=%~dp0docs\SFML-2.6.2-x64\SFML-2.6.2"

if not exist "%SFML_DIR%\include\SFML\Graphics.hpp" (
    echo [ERROR] SFML headers were not found in "%SFML_DIR%\include".
    echo Set SFML_DIR to the SFML 2.6.2 folder that contains include and lib.
    goto :failed
)

if not exist "%SFML_DIR%\lib\libsfml-graphics.a" if not exist "%SFML_DIR%\lib\libsfml-graphics.dll.a" (
    echo [ERROR] SFML MinGW libraries were not found in "%SFML_DIR%\lib".
    echo Install SFML 2.6.2 built for the same MinGW architecture as g++, then set SFML_DIR.
    goto :failed
)
if not exist "%SFML_DIR%\lib\libsfml-window.a" if not exist "%SFML_DIR%\lib\libsfml-window.dll.a" (
    echo [ERROR] SFML window library was not found in "%SFML_DIR%\lib".
    goto :failed
)
if not exist "%SFML_DIR%\lib\libsfml-system.a" if not exist "%SFML_DIR%\lib\libsfml-system.dll.a" (
    echo [ERROR] SFML system library was not found in "%SFML_DIR%\lib".
    goto :failed
)

rem Ensure SFML and MinGW runtime DLLs exist in the working directory
if not exist "%~dp0sfml-graphics-2.dll" if exist "%SFML_DIR%\bin\sfml-graphics-2.dll" (
    copy /y "%SFML_DIR%\bin\*.dll" "%~dp0" >nul 2>nul
)
if not exist "%~dp0libwinpthread-1.dll" if /i not "%CXX%"=="g++" for %%I in ("%CXX%") do (
    if exist "%%~dpIlibwinpthread-1.dll" copy /y "%%~dpIlibwinpthread-1.dll" "%~dp0" >nul 2>nul
    if exist "%%~dpIlibgcc_s_dw2-1.dll" copy /y "%%~dpIlibgcc_s_dw2-1.dll" "%~dp0" >nul 2>nul
    if exist "%%~dpIlibstdc++-6.dll" copy /y "%%~dpIlibstdc++-6.dll" "%~dp0" >nul 2>nul
)

rem Close running instance if any to prevent 'Permission denied' linking error
taskkill /F /IM main.exe >nul 2>nul

"%CXX%" -std=c++17 game\main.cpp game\logic\bst\bst.cpp game\logic\avl\avl_tree.cpp game\logic\heap\max_heap.cpp game\logic\event.cpp game\controller\LevelManager.cpp game\controller\UniversalActController.cpp game\scoring\ScoreSystem.cpp game\logic\SaveManager.cpp game\controller\act1-2\Act1State.cpp game\controller\act1-2\Act2State.cpp game\controller\act3-4\Act3State.cpp game\controller\act3-4\Act4State.cpp -I"%SFML_DIR%\include" -Igame -L"%SFML_DIR%\lib" -lsfml-graphics -lsfml-window -lsfml-system -static-libgcc -static-libstdc++ -o main.exe
if errorlevel 1 goto :failed

echo Build successful! Run with run.bat or .\main.exe
popd
exit /b 0

:failed
echo Build failed.
popd
exit /b 1
