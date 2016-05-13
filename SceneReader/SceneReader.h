#ifndef RAYTRACER_SCENEREADER_H
#define RAYTRACER_SCENEREADER_H

#include <vector>
#include "../Objects/Object3d.h"
#include "../LightSources/LightSource.h"

struct SceneData {
    SceneData() : camera(DEFAULT_CAMERA) { }

    SceneData(const Camera &camera, std::vector<Object3d *> &&objects, std::vector<LightSource *> &&lights) :
            camera(camera),
            objects(std::move(objects)),
            lights(std::move(lights)) { }

    SceneData(SceneData &&other) :
            camera(other.camera),
            objects(std::move(other.objects)),
            lights(std::move(other.lights)) { }

    void addObject(Object3d* &&object) {
        objects.push_back(object);
        object = nullptr;
    }

    void addMaterial(Material* &&material) {
        materials.push_back(material);
        material = nullptr;
    }

    void addLightSource(LightSource* &&lightSource) {
        lights.push_back(lightSource);
        lightSource = nullptr;
    }

    Camera camera;
    std::vector<Object3d *> objects;
    std::vector<LightSource *> lights;
    std::vector<Material *> materials;
};

class SceneReader {
public:
    virtual SceneData readScene(const std::string &path) = 0;
};


#endif //RAYTRACER_SCENEREADER_H
