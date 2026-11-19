cmake_minimum_required(VERSION 3.16)
project(ds_pro CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "E:/SFML-2.6.2/lib/cmake/SFML" CACHE PATH "Path to SFML CMake config")
find_package(SFML 2.6 COMPONENTS graphics window system audio network REQUIRED)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE sfml-graphics sfml-window sfml-system sfml-audio sfml-network)
target_link_options(main PRIVATE -static-libgcc -static-libstdc++)

