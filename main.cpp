#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "Picture.h"
#include "SceneReader/TextSTLReader.h"
#include "Objects/Sphere.h"

int main(int argc, char *argv[]) {
    TextSTLReader reader;
    CairoPainter cairoPainter(800, 600, "Ray Tracer");

    /// LIGHTS
    std::vector<LightSource *> lights;
//    lights.push_back(new LightSource(Point(10, 10, 0), 5000, CL_WHITE));
    lights.push_back(new LightSource(Point(20, 20, 0), 7000, CL_WHITE));
    lights.push_back(new LightSource(Point(-15, 0, 0), 200, CL_WHITE));

    /// STL MODELS
    std::vector<Object3d *> objects = reader.readObjects("./STLScenes/vvpfull.stl");
//    Scene::mergeObjects(objects, reader.readObjects("./STLScenes/floor.stl"));

    /// OTHER
    std::vector<Object3d *> manual = {
            new Sphere(Point(1, 1, 2), 1, Material(CL_RED, 0.8)),
            new Sphere(Point(-1, 1, 2), 1, Material(CL_BLUE, 0, 0.5, 0.7))
    };
//    Scene::mergeObjects(objects, std::move(manual));

    Picture picture;
    {
        Scene scene(Point(0, 0, 25),
                    Point(2, 1.5, 20), Point(-2, -1.5, 20),
                    800, 600,
                    std::move(objects),
                    std::move(lights));
        picture = scene.render();
    }
    cairoPainter.showPicture(picture);
    return 0;
}
