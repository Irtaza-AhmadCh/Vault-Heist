@echo off
echo Building Vault Heist: The Living Vault (Phase 4 Full Integration - Game Systems and Levels)...
g++ -std=c++17 src/main.cpp src/structures/bst/bst.cpp src/structures/avl/avl_tree.cpp src/structures/heap/max_heap.cpp src/core/events/event.cpp src/game/levels/LevelManager.cpp src/game/levels/UniversalActController.cpp src/game/scoring/ScoreSystem.cpp src/game/save/SaveManager.cpp src/game/levels/act1-2/Act1State.cpp -I"docs/SFML-2.6.2-x64/SFML-2.6.2/include" -L"docs/SFML-2.6.2-x64/SFML-2.6.2/lib" -lsfml-graphics -lsfml-window -lsfml-system -o main.exe
if %ERRORLEVEL% equ 0 (
    echo Build successful! Run with run.bat or .\main.exe
) else (
    echo Build failed.
)