#ifndef RAYTRACER_SCENEREADER_H
#define RAYTRACER_SCENEREADER_H

#include <vector>
#include "../Scene.h"

class SceneReader {
public:
    virtual SceneData readScene(const std::string &path) = 0;
};


#endif //RAYTRACER_SCENEREADER_H
