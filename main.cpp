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
    lights.push_back(new LightSource(Point(-1, 0, 0), 10, CL_WHITE));
    lights.push_back(new LightSource(Point(0, 0, 1), 1, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 0, 1), 4, CL_WHITE));
//    lights.push_back(new LightSource(Point(3, 2, 0), 10, CL_WHITE));
//    lights.push_back(new LightSource(Point(4, 10, 10), 1000, CL_WHITE));
//    lights.push_back(new LightSource(Point(-3, 0, 0), 5, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 3, 0), 30, CL_WHITE));
    std::vector<Object3d*> objects = reader.readObjects("./STLScenes/my.stl");
    objects.push_back(new Sphere(Point(-5, 0, -10), 5, Color(0, 0, 1), 0.3));
    objects.push_back(new Sphere(Point(-4, 0, 7), 2, CL_WHITE, 0.6));
    objects.push_back(new Sphere(Point(1, 0, 3), 2, CL_WHITE, 0));
//    objects.push_back(new Triangle3d(Point(-10, -10, 40), Point(10, -10, 40), Point(0, 10, 40), CL_WHITE, 0, 0.1));

    Scene scene(Point(0, 0, -5),
                Point(4, 3, 0), Point(-4, -3, 0),
                800, 600,
                objects,
                std::move(lights));
    Picture picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}