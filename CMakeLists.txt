cmake_minimum_required(VERSION 3.20)
project(rasterizer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build SDL3 as a static library so no runtime dylib is needed
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)

add_subdirectory(vendors/sdl)

add_executable(rasterizer
    src/main.c
    src/renderer/renderer.c
)

target_include_directories(rasterizer PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# SDL_MAIN_HANDLED prevents SDL from hijacking main() on every platform
target_compile_definitions(rasterizer PRIVATE SDL_MAIN_HANDLED)

target_link_libraries(rasterizer PRIVATE SDL3::SDL3-static)
