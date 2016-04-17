#include <fstream>
#include <exception>
#include "TextSTLReader.h"
#include "../Objects/Triangle3d.h"

std::vector<Object3d*> TextSTLReader::readObjects(const std::string &path) {
    std::vector<Object3d*> objects;
    std::ifstream input(path); // todo: decide what to do with exceptions.
    if (!input.is_open()) {
        throw std::invalid_argument("File not found");
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
            objects.push_back(new Triangle3d(a, b, c, color, reflectance));
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

