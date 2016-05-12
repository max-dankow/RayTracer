#ifndef RAYTRACER_STL_TEXT_READER_H
#define RAYTRACER_STL_TEXT_READER_H

#include <fstream>
#include <exception>
#include "../Scene.h"
#include "SceneReader.h"
#include "../Objects/Triangle3d.h"

// Считывает STL ASCII формат. В первую очередь предназначен для удобства человеку.
class TextSTLReader : public SceneReader {
public:
    std::vector<Object3d*> readObjects(const std::string &path);

    virtual SceneData readScene(const std::string &path);


};

SceneData TextSTLReader::readScene(const std::string &path) {
    return SceneData(readObjects(path), std::vector<LightSource *>());
}

std::vector<Object3d*> TextSTLReader::readObjects(const std::string &path) {
    std::vector<Object3d*> objects;
    std::ifstream input(path);
    if (!input) {
        throw std::invalid_argument("Can't read file");
    }
    std::cout << "Reading " << path  << std::endl;
    std::string word;
    // solid <name>
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    size_t count = 0;
    Color color(CL_WHITE);
    double reflectance = 0;

    while (!input.eof()) {
        // facet normal
        input >> word;
        if (word == "setcolor") {
            input >> color.r >> color.g >> color.b;
        }

        if (word == "setreflect") {
            input >> reflectance;
        }

        if (word == "facet") {
            count++;
            input >> word;
            Vector3d normal;
            input >> normal;
            // outer loop
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            Point a, b, c;
            input >> word >> a;
            input >> word >> b;
            input >> word >> c;

//            if (count % 2 == 0) {
//                color = Color(0.7, 0, 0);
//            } else {
//                color = Color(0, 0.7, 0);
//            }
            objects.push_back(new Triangle3d(a, b, c, &NO_MATERIAL));
            // endloop
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            // endfacet
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (word == "endsolid") {
            input.close();
            std::cout << "Read success: " << objects.size() << " primitives" << std::endl;
            return objects;
        }
    }
    assert(false);
}

#endif //RAYTRACER_STL_TEXT_READER_H
