#ifndef RAYTRACER_TESTSTLREADER_H
#define RAYTRACER_TESTSTLREADER_H


#include "../Scene.h"

// Считывает STL ASCII формат. В первую очередь предназначен для удобства человеку.
class TextSTLReader {
public:
    std::list<std::unique_ptr<Object3d>> readObjects(std::string path);
};


#endif //RAYTRACER_TESTSTLREADER_H
