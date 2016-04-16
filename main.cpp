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
    lights.push_back(new LightSource(Point(-6, 0, -4), 20, CL_WHITE));
//    lights.push_back(new LightSource(Point(20, 0, 7), 2, CL_WHITE));
//    lights.push_back(new LightSource(Point(-20, 0, 10), 20, CL_WHITE));
//    lights.push_back(new LightSource(Point(-9, 0, 7), 20, CL_WHITE));
//    lights.push_back(new LightSource(Point(9, 0, 7), 20, CL_WHITE));
//    lights.push_back(new LightSource(Point(9, 5, 15), 50, CL_WHITE));
//    lights.push_back(new LightSource(Point(2.5, 1.5, 0), 5, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 50, 30), 500, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 50, 50), 5000, CL_WHITE));

    Scene scene(Point(0, 0, 55),
                Point(4, 3, 50), Point(-4, -3, 50),
                800, 600, reader.readObjects("./STLScenes/invader.stl"),
                std::move(lights));
    Picture picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}