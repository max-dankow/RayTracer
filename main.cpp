#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "SceneReader/TextSTLReader.h"
#include "Objects/Sphere.h"
#include "Objects/Parallelogram.h"
#include "Objects/Triangle3d.h"
#include "LightSources/PointLight.h"
#include "PhotonMap.h"

// todo : перейти на float
int main(int argc, char *argv[]) {
    TextSTLReader reader;
    CairoPainter cairoPainter(800, 600, "Ray Tracer");

    /// LIGHTS
    std::vector<LightSource *> lights = {
            new PointLight(Point(0, 0, -5), 30, CL_WHITE),
//            new PointLight(Point(3, 2, 7), 2, CL_WHITE),
//            new PointLight(Point(-3, 3, 7), 20, CL_WHITE),
    };
    /// STL MODELS
    std::vector<Object3d *> objects;

//    objects = reader.readObjects("./STLScenes/alduin.stl");
    Scene::mergeObjects(objects, reader.readObjects("./STLScenes/floor.stl"));

    /// OTHER
    std::vector<Object3d *> manual = {
//            new Parallelogram({Point(40, -30, 50), Point(-40, -30, 50), Point(-40, 30, 50)}, Material(CL_WHITE * 0.5)),
//            new Triangle3d(Point(4, -4, -2.1), Point(-4, -4, -2.1), Point(0, 4, -2.1), Material(CL_WHITE, 0, 1, 1)),
            new Sphere(Point(3, 2, 7), 3, Material(CL_RED, 0.3)),
            new Sphere(Point(-3, 3, 7), 3, Material(CL_BLUE + CL_GREEN, 0, 1, 0.3))
    };
    Scene::mergeObjects(objects, std::move(manual));
//
//    PhotonMap map(lights, KdTree(std::vector<GeometricShape *>(objects.begin(), objects.end())), 1000000);
//
//    auto KNN = map.locatePhotons(Point(-4, 2, 4), 0.2, 1000);
//    for (Photon *photon : KNN) {
//        objects.push_back(new Sphere(photon->getRay().getOrigin(), 0.05, Material(CL_GREEN)));
//    }
//    for (Photon *photon : map.getStoredPhotons()) {
//        objects.push_back(new Sphere(photon->getRay().getOrigin(), 0.025, Material(photon->getColor())));
//    }

    Picture picture;
    Scene scene(Point(0, 0, -2),
                Point(2, 1.5, 0), Point(-2, -1.5, 0),
                800, 600,
                std::move(objects),
                std::move(lights), 1000000);
    picture = scene.render();

    cairoPainter.showPicture(picture);
    return 0;
}
