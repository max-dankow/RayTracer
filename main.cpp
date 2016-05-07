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
            new LightSource(Point(0, 4, -5), 20, CL_WHITE),
            new LightSource(Point(0, 20, 50), 1000, CL_WHITE),
    };
    /// STL MODELS
    std::vector<Object3d *> objects;
    objects = reader.readObjects("./STLScenes/alduin.stl");
    Scene::mergeObjects(objects, reader.readObjects("./STLScenes/floor.stl"));

    /// OTHER
    std::vector<Object3d *> manual = {
//            new Parallelogram({Point(40, -30, 50), Point(-40, -30, 50), Point(-40, 30, 50)}, Material(CL_WHITE * 0.5)),
//            new Triangle3d(Point(4, -4, 1), Point(-4, -4, 1), Point(0, 4, 1), Material(CL_GREEN, 0, 1, 0.8)),
            new Sphere(Point(3, 2, 7), 3, Material(CL_RED, 0.7)),
            new Sphere(Point(-3, 2, 7), 3, Material(CL_BLUE + CL_GREEN, 0, 0.5, 1))
    };
    Scene::mergeObjects(objects, std::move(manual));

    Picture picture;
    Scene scene(Point(0, 0, -2),
                Point(2, 1.5, 0), Point(-2, -1.5, 0),
                800, 600,
                std::move(objects),
                std::move(lights));
    picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}
