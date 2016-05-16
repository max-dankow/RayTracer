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
    std::time_t result = std::time(nullptr);
    std::string name(std::asctime(std::localtime(&result)));
    name.pop_back();
    return name + extension;
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

//    objects = reader.readObjects("./STLScenes/invader.stl");
    SceneData sceneData = readerRT.readScene("./RTScenes/caustest.tbd");

    Picture picture;
    Scene scene(std::move(sceneData), 5000000); // 5000000
    picture = scene.render(800, 600);

    CairoPainter cairoPainter(800, 600, "Ray Tracer");
    cairoPainter.showPicture(picture);
//    PNGPainter painter(800, 600, createFileName(".png"));
//    painter.showPicture(picture);
    return 0;
}
