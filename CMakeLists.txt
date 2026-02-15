cmake_minimum_required(VERSION 3.31)

set(CMAKE_TOOLCHAIN_FILE 
    "/home/bkmzk/.local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
)
set(CMAKE_CXX_STANDARD 
    20
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)

set(CMAKE_CXX_FLAGS "-march=x86-64 -mtune=generic -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

project(GAME)
project(engine)
project(gamesystem)

find_package(
    SFML COMPONENTS Graphics Window System 
    CONFIG REQUIRED
)
find_package(
    unofficial-noise 
    CONFIG REQUIRED
)
find_package(
    box2d 
    CONFIG REQUIRED
)

add_executable(
    GAME
    src/app/main.cpp
)

add_library(
    engine 
    SHARED
    src/lib/engine/storage.cpp
)

add_library(
    gamesystem
    STATIC
    src/lib/game/system.cpp
    src/lib/game/storage.cpp
)

target_link_libraries(
    engine PUBLIC 
    SFML::Graphics SFML::Window SFML::System
    unofficial::noise::noise-static
    box2d::box2d
)

target_link_libraries(
    gamesystem PUBLIC 
    engine
    SFML::Graphics SFML::Window SFML::System
)

target_link_libraries(
    GAME PRIVATE 
    engine
    gamesystem
    SFML::Graphics SFML::Window SFML::System
)

target_include_directories(
    engine
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(
    gamesystem 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
