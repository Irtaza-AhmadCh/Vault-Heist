@echo off
setlocal
echo ========================================================
echo   Compiling Vault Heist Benchmark Suite (Section 10)
echo ========================================================

g++ -std=c++17 -O3 -Wall -Wextra -mconsole -Wl,--stack,33554432 -I"%~dp0" benchmark.cpp game\logic\bst\bst.cpp game\logic\avl\avl_tree.cpp game\logic\heap\max_heap.cpp game\logic\event.cpp -o benchmark.exe

if errorlevel 1 (
    echo.
    echo [-] Benchmark compilation failed!
    exit /b 1
)

echo.
echo [+] Compilation successful! Running benchmark suite...
echo.

benchmark.exe > benchmark_results.txt
type benchmark_results.txt

echo.
echo ========================================================
echo   Benchmark Complete! Results saved to benchmark_results.txt
echo ========================================================
