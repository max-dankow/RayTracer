#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "Objects/Triangle3d.h"
#include "SceneReader/TextSTLReader.h"
#include "KdTree.h"
#include <thread>
using std::unique_ptr;

int main( int argc, char *argv[])
{
    TextSTLReader reader;
    CairoPainter cairoPainter(800, 600, "Ray Tracer");
    std::vector<std::unique_ptr<LightSource> > lights;
    lights.emplace_back(new LightSource(Point(0, 0, 0), 2, CL_WHITE));
    lights.emplace_back(new LightSource(Point(2.5, 1.5, 0), 5, CL_WHITE));
    KdTree kdTree(reader.readObjects("./STLScenes/my.stl"));

//    Scene scene(Point(1, 1, -5),
//                Point(4, 3, 0), Point(-4, -3, 0),
//                800, 600, reader.readObjects("./STLScenes/my.stl"), std::move(lights));
//
//    Picture picture = scene.render();
//
//    cairoPainter.showPicture(picture);
    return 0;
}