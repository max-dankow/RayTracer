#include <iostream>
#include "Painter/CairoPainter.h"
#include "Scene.h"
#include "SceneReader/TextSTLReader.h"
#include "SceneReader/RTReader.h"
#include "ArgumentsReader.h"
#include "Painter/PNGPainter.h"

std::string createFileName(const std::string &path, const std::string &extension) {
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

SceneData readFiles(const std::vector<std::string> &paths) {
    SceneData sceneData;
    if (paths.empty()) {
        return sceneData;
    }
    bool theFirst = true;
    for (std::string path : paths) {
        std::string extension;
        if (path.find_last_of(".") != std::string::npos) {
            extension = path.substr(path.find_last_of(".") + 1);
        }
        SceneData currentFileData;
        bool success = false;
        bool supportedFormat = false;
        if (extension == "stl") {
            supportedFormat = true;
            try {
                currentFileData = TextSTLReader::readScene(path);
                success = true;
            } catch (std::invalid_argument &e) {
                std::cerr << e.what() << ' ' << path << std::endl;
            }
        }
        if (extension == "rt" || extension == "tbd") {
            supportedFormat = true;
            try {
                currentFileData = RTReader::readScene(path);
                success = true;
            } catch (std::invalid_argument &e) {
                std::cerr << e.what() << ' ' << path << std::endl;
            }
        }
        if (!success) {
            if (!supportedFormat) {
                std::cerr << "Unsupported format " << extension << std::endl;
            }
            continue;
        }
        if (theFirst) {
            theFirst = false;
            sceneData = std::move(currentFileData);
        } else {
            sceneData.merge(std::move(currentFileData));
        }
    }
    return sceneData;
}

// todo : перейти на float
int main(int argc, char *argv[]) {
    Arguments arguments = ArgumentsReader::readArguments(argc, argv);
//    ArgumentsReader::printHelp(std::cout);
    if (arguments.helpFlag) {
        return 0;
    }
    printSizes();
    SceneData sceneData = readFiles(arguments.files);

    Scene scene(std::move(sceneData), arguments.sceneProperties);
    auto picture = scene.render(arguments.pictureWidth, arguments.pictureHeight);

//    CairoPainter cairoPainter(arguments.pictureWidth, arguments.pictureHeight, "Ray Tracer");
//    cairoPainter.showPicture(picture);
    PNGPainter painter(arguments.pictureWidth, arguments.pictureHeight, createFileName("./results/", ".png"));
    painter.showPicture(picture);
    return 0;
}
