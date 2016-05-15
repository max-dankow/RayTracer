#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "SceneReader/TextSTLReader.h"
#include "Objects/Sphere.h"
#include "Objects/Parallelogram.h"
#include "Objects/Triangle3d.h"
#include "LightSources/PointLight.h"
#include "PhotonMap.h"
#include "SceneReader/RTReader.h"
#include <ctime>
#include "Painter/PNGPainter.h"

std::string createFileName(const std::string &extension) {
    std::string path = "./results/";
    std::time_t result = std::time(nullptr);
    std::string name(std::asctime(std::localtime(&result)));
    name.pop_back();
    return path + name + extension;
}

void printSizes() {
    std::cout << "Triangle " << sizeof(Triangle3d) << "\n";
    std::cout << "Vector " << sizeof(Vector3d) << "\n";
    std::cout << "BoundingBox " << sizeof(BoundingBox) << "\n";
    std::cout << "Ray " << sizeof(Ray) << "\n";
    std::cout << "Photon " << sizeof(Photon) << "\n";
    std::cout << "KD Node " << sizeof(KdNode) << "\n";
    std::cout << "Color " << sizeof(Color) << "\n";
}

// todo : перейти на float
int main(int argc, char *argv[]) {
    printSizes();
    RTReader readerRT;
    TextSTLReader reader;
    /// LIGHTS
//    std::vector<LightSource *> lights = {
////            new PointLight(Point(5, 1, 0), 25, CL_RED),
////            new PointLight(Point(-5, 1, 0), 25, CL_GREEN),
//            new PointLight(Point(0, 1, 0), 10, CL_WHITE),
//            new PointLight(Point(0, 0, 30), 30, CL_WHITE),
////            new PointLight(Point(-3, 3, 7), 20, CL_WHITE),
//    };
    /// STL MODELS
    std::vector<Object3d *> objects;

//    objects = reader.readObjects("./STLScenes/invader.stl");
    SceneData sceneData = readerRT.readScene("./RTScenes/caustest.tbd");
    Scene::mergeObjects(objects, std::move(sceneData.objects));

    /// OTHER
//    std::vector<Object3d *> manual = {
//            new Parallelogram({Point(3, 2, 3), Point(-3, 2, 3), Point(-3, 2, 1)}, Material(CL_WHITE, 1)),
//            new Parallelogram({Point(3, -1, 25), Point(-3, -1, 25), Point(-3, 0, 25)}, Material(CL_WHITE)),
//            new Triangle3d(Point(2, -1, 25), Point(0, -1, 25), Point(0, 1, 25), new Material(CL_BLUE, 0, 1, 0.8)),
//            new Triangle3d(Point(0, -1, 25), Point(-2, -1, 25), Point(0, 1, 25), new Material(CL_RED, 0, 1, 0.8)),
//            new Triangle3d(Point(2, 1, 25), Point(2, -1, 25), Point(0, 1, 25), new Material(CL_BLUE + CL_RED, 0, 1, 1)),
//            new Triangle3d(Point(0, 1, 25), Point(-2, -1, 25), Point(-2, 1, 25), new Material(CL_GREEN, 0, 1, 0.3)),
//            new Sphere(Point(3, 2, 13), 3, new Material(CL_RED, 0.9)),
//            new Sphere(Point(-3, 2, 16), 3, new Material(CL_RED + CL_GREEN, 0, 1, 0.7))
//    };
//    Scene::mergeObjects(objects, std::move(manual));


//    PhotonMap map(lights, KdTree(std::vector<GeometricShape *>(objects.begin(), objects.end())), 10000);
//
//    for (Photon *photon : map.getStoredPhotons()) {
//        objects.push_back(new Sphere(photon->getRay().getOrigin(), 0.05, Material(photon->getColor())));
//    }

    Picture picture;
    Scene scene(sceneData.camera,
                std::move(objects), // todo: consume SceneData
                std::move(sceneData.lights),
                5000000); // 5000000
    picture = scene.render(800, 600);

//    CairoPainter cairoPainter(800, 600, "Ray Tracer");
//    cairoPainter.showPicture(picture);
    PNGPainter painter(800, 600, createFileName(".png"));
    painter.showPicture(picture);
    return 0;
}
