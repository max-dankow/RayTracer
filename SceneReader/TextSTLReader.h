#ifndef RAYTRACER_STL_TEXT_READER_H
#define RAYTRACER_STL_TEXT_READER_H

#include <fstream>
#include <exception>
#include "../Scene.h"
#include "../Objects/Triangle3d.h"

// Считывает STL ASCII формат. Предназначен для печати на 3d принтере.
class TextSTLReader {
public:
    static SceneData readScene(const std::string &path) {
        SceneData sceneData;
        std::ifstream input(path);
        if (!input) {
            throw std::invalid_argument("Can't read file");
        }
        std::cout << "Reading " << path << std::endl;
        std::string word;
        // solid <name>
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        size_t count = 0;

        while (!input.eof()) {
            // facet normal
            input >> word;

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

//                if (count % 2 == 0) {
//                    color = Color(0.7, 0, 0);
//                } else {
//                    color = Color(0, 0.7, 0);
//                }

                sceneData.addObject(new Triangle3d(a, b, c, &NO_MATERIAL));
                // endloop
                input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                // endfacet
                input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            if (word == "endsolid") {
                break;
            }
        }
        input.close();
        std::cout << "Read success: " << sceneData.objects.size() << " primitives" << std::endl;
        return sceneData;
    }
};

#endif //RAYTRACER_STL_TEXT_READER_H
