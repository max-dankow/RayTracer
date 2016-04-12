#ifndef RAYTRACER_TESTSTLREADER_H
#define RAYTRACER_TESTSTLREADER_H

#include "../Scene.h"

// Считывает STL ASCII формат. В первую очередь предназначен для удобства человеку.
class TextSTLReader {
public:
    std::vector<Object3d*> readObjects(const std::string &path);
};

#endif //RAYTRACER_TESTSTLREADER_H
