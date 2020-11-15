cmake_minimum_required(VERSION 3.17)
project(mandelbrot C)

set(CMAKE_C_STANDARD 11)

add_executable(mandelbrot main.c  mandelbrot.c mandelbrot.h)
add_library(mandelbrot_lib  mandelbrot.c mandelbrot.h)

include_directories(~/CLionProjects/graphics)

find_library(LibGraphics libgraphics.a ~/CLionProjects/graphics/cmake-build-debug/)

target_link_libraries(mandelbrot m ${LibGraphics})
target_link_libraries(mandelbrot_lib m ${LibGraphics})