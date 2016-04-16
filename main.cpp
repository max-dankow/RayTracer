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
    std::vector<LightSource*> lights;
//    lights.push_back(new LightSource(Point(-20, 30, -10), 1000, CL_WHITE));
    lights.push_back(new LightSource(Point(20, -9, 30), 600, CL_WHITE));
    lights.push_back(new LightSource(Point(0, 0, 20), 100, CL_WHITE));
    lights.push_back(new LightSource(Point(0, 20, 10), 1000, CL_WHITE));

    Scene scene(Point(0, 0, 5),
                Point(4, 3, 0), Point(-4, -3, 0),
                800, 600, reader.readObjects("./STLScenes/golum.stl"),
                std::move(lights));
    Picture picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}