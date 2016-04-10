#include <fstream>
#include <exception>
#include "TextSTLReader.h"
#include "../Objects/Triangle3d.h"

std::vector<std::unique_ptr<Object3d>> TextSTLReader::readObjects(std::string path) {
    std::vector<std::unique_ptr<Object3d>> objects;
    std::ifstream input(path); // todo: decide what to do with exceptions.
    if (!input.is_open()) {
        throw std::invalid_argument("File not found");
    }
    std::string word;
    // solid <name>
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (!input.eof()) {
        // facet normal
        input >> word;

        if (word == "facet") {
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

            objects.emplace_back(new Triangle3d(a, b, c, CL_WHITE));
            // endloop
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            // endfacet
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (word == "endsolid") {
            input.close();
            std::cout << "Read success: " << objects.size() << " vertices" << std::endl;
            return objects;
        }
    }
    assert(false);
}

