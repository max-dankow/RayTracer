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
//    objects.emplace_back(new Triangle3d(Point(0, 0, 2), Point(0, 1, 1), Point(1, 0, 1), Color(1, 0, 0)));
//    objects.emplace_back(new Triangle3d(Point(-2, 1, 2), Point(2, 1, 2), Point(0, -2, 2), Color(0, 1, 0)));
    objects.emplace_back(new Triangle3d(Point(-4, 0, 2), Point(-4, 0, 4), Point(-4, 5, 3), Color(0, 1, 0)));
    objects.emplace_back(new Triangle3d(Point(0, 0, 2), Point(0, 0, 4), Point(0, 5, 3), Color(0, 1, 0)));
    objects.emplace_back(new Triangle3d(Point(2, 0, 2), Point(2, 0, 4), Point(2, 5, 3), Color(0, 1, 0)));

    Scene scene(Point(0, 0, -1),
                Point(-4, 3, 0), Point(4, -3, 0),
                800, 600, std::move(objects));
    Picture picture = scene.render();
    picture.setAt(79, 59, Color(1, 1, 0));


    cairoPainter.showPicture(picture);
    return 0;
}