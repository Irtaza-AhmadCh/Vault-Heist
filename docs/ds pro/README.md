# SFML 2.6.2 Project (DS Pro)

This project is configured with **SFML 2.6.2** using the compatible **MinGW-w64 GCC 13.1.0 (32-bit Dwarf)** toolchain.

---

## Environment Paths
- **SFML Directory:** `E:\SFML-2.6.2`
- **Compiler:** `E:\mingw32\bin\g++.exe` (GCC 13.1.0 i686)
- **Make Tool:** `E:\mingw32\bin\mingw32-make.exe`
- **Debugger:** `E:\mingw32\bin\gdb.exe`

---

## Project Structure
```
ds pro/
├── .vscode/
│   ├── c_cpp_properties.json   # IntelliSense & include path configuration
│   ├── tasks.json               # Build & run tasks for VS Code
│   └── launch.json              # GDB debugging configuration (F5)
├── src/
│   └── main.cpp                 # SFML entry point & interactive demo
├── CMakeLists.txt               # CMake configuration
├── Makefile                     # Make configuration
├── build.bat                    # One-click build script
├── run.bat                      # One-click run script
├── *.dll                        # SFML and GCC runtime DLLs
└── .gitignore
```

---

## How to Build & Run

### 1. Using Batch Scripts (Easiest)
- Double-click or run:
  ```powershell
  .\run.bat
  ```
  *(Automatically builds `src/main.cpp` if needed and starts the application).*

- To rebuild only:
  ```powershell
  .\build.bat
  ```

### 2. Using VS Code
- **Build:** Press `Ctrl + Shift + B` (runs default build task).
- **Debug:** Press `F5` to start debugging with GDB.

### 3. Using Make
```powershell
E:\mingw32\bin\mingw32-make.exe all
.\main.exe
```

### 4. Using CMake
```powershell
E:\mingw32\bin\cmake.exe -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="E:/mingw32/bin/g++.exe" -DCMAKE_MAKE_PROGRAM="E:/mingw32/bin/mingw32-make.exe"
E:\mingw32\bin\cmake.exe --build build
```

---

## Controls for the Demo App
- **Spacebar:** Cycle circle colors.
- **Left Mouse Click:** Move circle to cursor position.
- **Escape / Close Window:** Exit.

