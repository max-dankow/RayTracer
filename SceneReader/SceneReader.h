#ifndef RAYTRACER_SCENEREADER_H
#define RAYTRACER_SCENEREADER_H

#include <vector>
#include "../Objects/Object3d.h"
#include "../LightSources/LightSource.h"

class SceneData {
public:
    SceneData() { }

    SceneData(const std::vector<Object3d *> &&objects, std::vector<LightSource *> &&lights) :
            objects(std::move(objects)),
            lights(std::move(lights)) { }

    SceneData(SceneData &&other) :
            objects(std::move(other.objects)),
            lights(std::move(other.lights)) { }

    void addObject(Object3d* &&object) {
        objects.push_back(object);
        object = nullptr;
    }

    std::vector<Object3d *> &getObjects() {
        return objects;
    }

private:
    std::vector<Object3d *> objects;
    std::vector<LightSource *> lights;
};

class SceneReader {
public:
    virtual SceneData readScene(const std::string &path) = 0;
};


#endif //RAYTRACER_SCENEREADER_H
