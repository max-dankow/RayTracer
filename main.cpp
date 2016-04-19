#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "Objects/Triangle3d.h"
#include "SceneReader/TextSTLReader.h"
#include "KdTree.h"
#include <thread>
#include "Objects/Sphere.h"

using std::unique_ptr;

int main( int argc, char *argv[])
{
    TextSTLReader reader;
    CairoPainter cairoPainter(800, 600, "Ray Tracer");
    std::vector<LightSource*> lights;
//    lights.push_back(new LightSource(Point(-3, 1, 0), 20, CL_WHITE));
//    lights.push_back(new LightSource(Point(5, 5, 20), 1000, CL_WHITE));
//    lights.push_back(new LightSource(Point(-3, 0, 0), 5, CL_WHITE));
//    lights.push_back(new LightSource(Point(0, 0, -5), 1, CL_WHITE));
    lights.push_back(new LightSource(Point(-5, 5, 0), 1, CL_WHITE));

    std::vector<Object3d*> objects = reader.readObjects("./STLScenes/vader.stl");
//    objects.push_back(new Sphere(Point(-5, -5, 15), 5, Color(1, 0, 0), 0.2));
//    objects.push_back(new Sphere(Point(5, -5, 15), 5, Color(0, 1, 0), 0.7));
//    objects.push_back(new Sphere(Point(0, 3, 15), 5, Color(0, 0, 1), 0.9));
//    objects.push_back(new Sphere(Point(0, 0, 20), 5, CL_WHITE, 0));
//    objects.push_back(new Triangle3d(Point(-100, -50, -100), Point(-10, 50, -100), Point(100, -50, -100), CL_WHITE, 0));

    Scene scene(Point(0, 0, 25),
                Point(4, 3, 20), Point(-4, -3, 20),
                800, 600,
                /*reader.readObjects("./STLScenes/golum.stl")*/objects,
                lights);
    Picture picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}