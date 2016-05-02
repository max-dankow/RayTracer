cmake_minimum_required(VERSION 3.4)
project(RayTracer)
set(PAINTER_NAME "CAIRO")

find_package(PkgConfig REQUIRED)

if(${PAINTER_NAME} STREQUAL "CAIRO")
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})

    pkg_check_modules(CAIRO REQUIRED cairo)
    include_directories(${CAIRO_INCLUDE_DIRS})
    link_directories(${CAIRO_LIBRARY_DIRS})
    set(PAINTER_LIBS ${CAIRO_LIBRARIES} ${GTK3_LIBRARIES})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pthread")

set(SOURCE_FILES main.cpp Painter/Painter.h Painter/CairoPainter.h Painter/CairoPainter.cpp
        Objects/Object3d.h Scene.cpp Scene.h Objects/Triangle3d.cpp Objects/Triangle3d.h
        Geometry/Geometry.h Picture.h LightSource.h
        SceneReader/TextSTLReader.cpp SceneReader/TextSTLReader.h
        KdTree.cpp KdTree.h Geometry/Vector3d.h Geometry/Ray.h Geometry/BoundingBox.h Geometry/BoundingBox.cpp Objects/Sphere.cpp Objects/Sphere.h Objects/PlaneQuadrangle.h Objects/Parallelogram.h)
set(TEST_FILES Painter/Painter.h Painter/CairoPainter.h Painter/CairoPainter.cpp Objects/Object3d.h Scene.cpp Scene.h Objects/Triangle3d.cpp Objects/Triangle3d.h Geometry/Ray.h Geometry/BoundingBox.h Geometry/BoundingBox.cpp Geometry/Geometry.h Tests/Tests.cpp Objects/PlaneQuadrangle.h Objects/Parallelogram.h)
add_executable(RayTracer ${SOURCE_FILES})
add_executable(Tests ${TEST_FILES})
target_link_libraries(RayTracer ${PAINTER_LIBS})
target_link_libraries(Tests ${PAINTER_LIBS} gtest)