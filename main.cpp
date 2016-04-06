#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "Objects/Triangle3d.h"
#include "SceneReader/TextSTLReader.h"
#include <thread>
//
//void window(CairoPainter& cairoPainter) {
//    cairoPainter.show();
//}
using std::unique_ptr;

int main( int argc, char *argv[])
{
    TextSTLReader reader;
//    reader.readObjects("Star.stl");
    CairoPainter cairoPainter(800, 600, "Ray Tracer");
//    std::list<std::unique_ptr<Object3d> > objects;
//    objects.emplace_back(new Triangle3d(Point(-8, -4, 7), Point(0, 4, 10), Point(8, -4, 1), Color(0.6, 0.9, 0)));
//    objects.emplace_back(new Triangle3d(Point(-0.3, -0.3, 0.5), Point(0, 0, 0.5), Point(0, -0.3, 0.5), Color(0, 1, 0)));
//    objects.emplace_back(new Triangle3d(Point(-0.3+3, -1.4, 2), Point(3, -1.1, 2), Point(3, -1.4, 2), Color(0, 1, 0)));
//    objects.emplace_back(new Triangle3d(Point(-2, 1, 2), Point(2, 1, 2), Point(-1, -2, 2), Color(0, 1, 0)));
//    objects.emplace_back(new Triangle3d(Point(-4, 0, 2), Point(-4, 0, 4), Point(-4, 5, 3), Color(0, 0.5, 0)));
//    objects.emplace_back(new Triangle3d(Point(0, 0, 2), Point(0, 0, 4), Point(0, 5, 3), Color(0.7, 1, 0.5)));
//    objects.emplace_back(new Triangle3d(Point(1, 0, 1), Point(1, 0, 4), Point(1, 5, 3), Color(0.7, 1, 0)));
//    objects.emplace_back(new Triangle3d(Point(-3, 0, 1), Point(-3, 0, 4), Point(-3, 5, 3), Color(0.7, 1, 0)));
    std::list<std::unique_ptr<LightSource> > lights;
//    lights.emplace_back(new LightSource(Point(0, -1, 3), CL_WHITE));
//    lights.emplace_back(new LightSource(Point(0, 0, 0), CL_WHITE));
    lights.emplace_back(new LightSource(Point(-1, 0, 0), CL_WHITE));
//    lights.emplace_back(new LightSource(Point(8, -4, 0), CL_WHITE));

    Scene scene(Point(0, -3, -5),
                Point(4, 3, 0), Point(-4, -3, 0),
                800, 600, reader.readObjects("./STLScenes/Moon.stl"), std::move(lights));
//    scene.emplaceObject(new Triangle3d(Point(-0.3 + 3, -1.4, 2), Point(3, -1.1, 2), Point(3, -1.4, 2), Color(0, 1, 0)));
    Picture picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}