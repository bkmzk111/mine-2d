cmake_minimum_required(VERSION 3.31)

set(CMAKE_TOOLCHAIN_FILE 
    "D:/Visual Studio BT/2022/VC/vcpkg/scripts/buildsystems/vcpkg.cmake"
)
set(CMAKE_CXX_STANDARD 
    20
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)

project(GAME)
project(LIB)

find_package(
    SFML COMPONENTS
    Graphics Window Audio System 
    CONFIG REQUIRED
)

add_executable(
    GAME
    src/app/main.cpp
)
add_library(
    LIB 
    SHARED
    src/lib/engine.cpp
)


target_link_libraries(
    LIB PRIVATE 
    SFML::Graphics SFML::Window SFML::Audio SFML::System
)
target_link_libraries(
    GAME PRIVATE 
    SFML::Graphics SFML::Window SFML::Audio SFML::System
)

target_link_libraries(
    GAME PRIVATE 
    LIB
)

target_include_directories(
    LIB 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
