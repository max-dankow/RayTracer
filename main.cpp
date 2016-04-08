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
    CairoPainter cairoPainter(800, 600, "Ray Tracer");
    std::list<std::unique_ptr<LightSource> > lights;
    lights.emplace_back(new LightSource(Point(0, 0, 0), 2, CL_WHITE));
    lights.emplace_back(new LightSource(Point(2.5, 1.5, 0), 5, CL_WHITE));

    Scene scene(Point(1, 1, -5),
                Point(4, 3, 0), Point(-4, -3, 0),
                800, 600, reader.readObjects("./STLScenes/Moon.stl"), std::move(lights));

    Picture picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}