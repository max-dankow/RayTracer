#include <fstream>
#include <exception>
#include "TextSTLReader.h"
#include "../Objects/Triangle3d.h"

std::list<std::unique_ptr<Object3d>> TextSTLReader::readObjects(std::string path) {
    std::list<std::unique_ptr<Object3d>> objects;
    std::ifstream input(path); // todo: decide what to do with exceptions.
    std::string word;
    // solid <name>
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (!input.eof()) {
        // facet normal
        input >> word;
        std::cout<< "main = " << word << '\n';

        if (word == "facet") {
            input >> word;
            Vector3d normal;
            input >> normal;
            // outer loop
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            Point a, b, c;
            input >> word >> a;
            std::cout << word << '\n';

            input >> word >> b;
            std::cout << word << '\n';

            input >> word >> c;
            std::cout << word << '\n';

            objects.emplace_back(new Triangle3d(a, b, c, CL_WHITE));
            // endloop
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            // endfacet
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (word == "endsolid") {
            input.close();
            return objects;
        }
    }
    assert(false);

    //
    /*int state = 0;
    do {
        std::string word;
        std::cout << word;
        switch (state) {
            case 0: {
                if (word == "solid") {
                    state = 1;
                }
                input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // todo: check if it is correct and crossplatform
            }
            case 1: {
                if (word == "facet") {
                    state = 2;
                }
            }
            case 2: {
                if (word == "normal") {

                }
            }
            default: throw std::invalid_argument("Invalid STL ASCII file");
        }
    } while (state != -1);*/
//    input.close();
}

