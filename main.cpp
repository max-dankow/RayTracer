#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "Objects/Triangle3d.h"
#include <thread>
//
//void window(CairoPainter& cairoPainter) {
//    cairoPainter.show();
//}

int main( int argc, char *argv[])
{
    CairoPainter cairoPainter(800, 600, "Ray Tracer");
    std::list<std::unique_ptr<Object3d> > objects;
    objects.emplace_back(new Triangle3d(Point(-8, -4, 5), Point(0, 4, 5), Point(8, -4, 5), Color(1, 0, 0)));
    objects.emplace_back(new Triangle3d(Point(-0.3, -3, 6), Point(0.5, 0, 0.5), Point(0, 0.5, 0.5), Color(0, 1, 0)));
//    objects.emplace_back(new Triangle3d(Point(-2, 1, 2), Point(2, 1, 2), Point(-1, -2, 2), Color(0, 1, 0)));
//    objects.emplace_back(new Triangle3d(Point(-4, 0, 2), Point(-4, 0, 4), Point(-4, 5, 3), Color(0, 0.5, 0)));
//    objects.emplace_back(new Triangle3d(Point(0, 0, 2), Point(0, 0, 4), Point(0, 5, 3), Color(0.7, 1, 0.5)));
//    objects.emplace_back(new Triangle3d(Point(1, 0, 1), Point(1, 0, 4), Point(1, 5, 3), Color(0.7, 1, 0)));
//    objects.emplace_back(new Triangle3d(Point(-3, 0, 1), Point(-3, 0, 4), Point(-3, 5, 3), Color(0.7, 1, 0)));
    std::list<std::unique_ptr<LightSource> > lights;
    lights.emplace_back(new LightSource(Point(0, 0, -3), CL_WHITE));
//    lights.emplace_back(new LightSource(Point(2, 0, -3), CL_WHITE));

    Scene scene(Point(0, 0, -5),
                Point(4, 3, 0), Point(-4, -3, 0),
                800, 600, std::move(objects), std::move(lights));
    Picture picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}