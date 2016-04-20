#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "Objects/Triangle3d.h"
#include "SceneReader/TextSTLReader.h"
#include "KdTree.h"
#include "Objects/Sphere.h"
#include <thread>
using std::unique_ptr;

int main( int argc, char *argv[])
{
    TextSTLReader reader;
    CairoPainter cairoPainter(800, 600, "Ray Tracer");
    std::vector<LightSource*> lights;
//    lights.push_back(new LightSource(Point(-3, 1, 0), 20, CL_WHITE));
//    lights.push_back(new LightSource(Point(-1, 0, -2), 3, CL_WHITE));
//    lights.push_back(new LightSource(Point(-5, 3, 3), 10, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 0, 0.5), 2, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 0, 1.5), 5, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 0, 2), 4, CL_WHITE));
//    lights.push_back(new LightSource(Point(3, 2, 0), 10, CL_WHITE));
    lights.push_back(new LightSource(Point(0, 0, 50), 500, CL_WHITE));
    lights.push_back(new LightSource(Point(-10, 10, 0), 1200, CL_WHITE));
//    lights.push_back(new LightSource(Point(-3, 0, 0), 5, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 3, 0), 30, CL_WHITE));
    std::vector<Object3d*> objects = reader.readObjects("./STLScenes/golum.stl");
    objects.push_back(new Sphere(Point(0, 0, 5), 10, Material(CL_WHITE, 0, 0.5, 0.8)));
//    objects.push_back(new Sphere(Point(-4, 0, 7), 2, Material(CL_WHITE, 0.6)));
//    objects.push_back(new Sphere(Point(0, 0, 2), 2, Material(CL_GREEN, 0, 0.5, 0.8)));
//    objects.push_back(new Triangle3d(Point(-4, -2, 1), Point(-2, 2, 1), Point(0, -2, 1), Material(CL_WHITE, 0, 0.5, 1)));

    Scene scene(Point(0, 0, 35),
                Point(4, 3, 30), Point(-4, -3, 30),
                800, 600,
                objects,
                std::move(lights));
    Picture picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}