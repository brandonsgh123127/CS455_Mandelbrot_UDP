cmake_minimum_required(VERSION 3.17)
include_directories(/usr/include/X11)
project(graphics C)

set(CMAKE_C_STANDARD 11)
add_library(graphics graphicslibrary.c graphicslibrary.h xwindows_utilities.c xwindows_utilities.h)
add_executable(simpleGraphics main.c graphicslibrary.c graphicslibrary.h xwindows_utilities.c xwindows_utilities.h)
target_link_libraries(simpleGraphics graphics X11 m)