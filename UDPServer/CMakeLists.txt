cmake_minimum_required(VERSION 3.17)
project(UDPServer C)

set(CMAKE_C_STANDARD 11)
include_directories(~/CLionProjects/graphics)
include_directories(~/CLionProjects/mandelbrot)
find_library(LibGraphics libgraphics.a ~/CLionProjects/graphics/cmake-build-debug/)
find_library(LibMandelbrot libmandelbrot_lib.a ~/CLionProjects/mandelbrot/cmake-build-debug/)

add_executable(UDPServer main.c UDP_Routines.c UDP_Routines.h)
target_link_libraries(UDPServer pthread)
target_link_libraries(UDPServer m)
target_link_libraries(UDPServer ${LibMandelbrot} ${LibGraphics} m)