cmake_minimum_required(VERSION 2.8)
project(RayTracer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(SOURCE_FILES main.cpp Painter/Painter.h Painter/CairoPainter.h)
add_executable(RayTracer ${SOURCE_FILES})
target_link_libraries(RayTracer cairo)