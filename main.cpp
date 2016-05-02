#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "Objects/PlaneQuadrangle.h"
#include "SceneReader/TextSTLReader.h"
#include "Objects/Sphere.h"
#include "Objects/Parallelogram.h"
#include "Objects/Triangle3d.h"

// todo : перейти на float
int main(int argc, char *argv[]) {
    TextSTLReader reader;
    CairoPainter cairoPainter(800, 600, "Ray Tracer");

    /// LIGHTS
    std::vector<LightSource *> lights = {
            new LightSource(Point(0, 0, -5), 20, CL_WHITE),
//            new LightSource(Point(-10, 0, 30), 1000, CL_WHITE)
    };
    /// STL MODELS
    std::vector<Object3d *> objects;// = reader.readObjects("./STLScenes/my.stl");
    Scene::mergeObjects(objects, reader.readObjects("./STLScenes/floor.stl"));

    /// OTHER
    std::vector<Object3d *> manual = {
//            new PlaneQuadrangle({Point(1, -1, 2), Point(-3, -0.7, 2), Point(-1, 1, 2), Point(0, 1, 2)}, Material(CL_GREEN)),
            new Parallelogram({Point(1, -1, 3), Point(-2, -1, 3), Point(-1, 1, 3)}, Material(CL_BLUE)),
//            new Triangle3d(Point(1, -1, 2), Point(-2, -2, 2), Point(-1, 0, 2))
//            new Sphere(Point(1, 1, 2), 1, Material(CL_RED, 0.8)),
//            new Sphere(Point(-1, 1, 2), 1, Material(CL_BLUE, 0, 0.5, 0.7))
    };
    Scene::mergeObjects(objects, std::move(manual));

    Picture picture;
    Scene scene(Point(0, 0, -5),
                Point(2, 1.5, -2), Point(-2, -1.5, -2),
                800, 600,
                std::move(objects),
                std::move(lights));
    picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}
