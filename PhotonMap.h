#ifndef RAYTRACER_PHOTONMAP_H
#define RAYTRACER_PHOTONMAP_H

#include <vector>
#include "KdTree.h"
#include "LightSources/LightSource.h"
#include "Objects/Object3d.h"

class PhotonMap {
public:
    PhotonMap(const std::vector<LightSource *> &lights, const KdTree &kdTree, unsigned long photonNumber) {
        // todo: надписи и progress bar
        double totalPower = 0;
        for (LightSource *light : lights) {
            totalPower += light->getBrightness();
        }
        // Чтобы не делить на 0, если вообще нет света, выходим.
        if (Geometry::areDoubleEqual(totalPower, 0)) {
            return;
        }
        // Генерация и трассировка фотонов.
        for (LightSource *light : lights) {
            long photonsPerThisLight = long (light->getBrightness() * photonNumber / totalPower);
            for (long i = 0; i < photonsPerThisLight; ++i) {
                auto photon = light->emitPhoton();
                tracePhoton(kdTree, photon, 2);
            }
        }
        // Построение фотонного kd-дерева.
    }


    const std::vector<Photon> &getStoredPhotons() const {
        return storedPhotons;
    }

private:
    void tracePhoton(const KdTree &objects, const Photon &photon, int restDepth) {
        if (restDepth <= 0) {
            return;
        }
        // Используя Kd дерево ищем пересечения со сценой.
        Point hitPoint;
        Object3d *obstacle = dynamic_cast<Object3d *> (objects.findObstacle(photon.getRay(), hitPoint));

        // Если не нашли пересечений.
        if (obstacle == nullptr) {
            return;
        }
        auto obstacleMaterial = obstacle->getMaterial();
        // Русская рулетка.
        std::uniform_real_distribution<float> distribution(0, 1);
        float randomVariable = distribution(randomEngine);
        if (randomVariable < obstacleMaterial.reflectance) {
            // Решается, что луч отразился.
            auto reflectedDirection = photon.getRay().reflectRay(obstacle->getNormal(hitPoint));
            Ray reflectedRay(hitPoint, reflectedDirection);
            reflectedRay.push();
            Photon reflectedPhoton(reflectedRay, photon.getColor());
            tracePhoton(objects, reflectedPhoton, restDepth - 1);
        } else {
//            if (randomVariable < obstacleMaterial.reflectance + obstacleMaterial.transparency) {
//                // Решается, что луч преломился.
//            }

            storedPhotons.push_back(Photon(hitPoint, photon.getDirection(), photon.getColor()));
        }

    }

    KdTree generalTree;
    std::vector<Photon> storedPhotons;
    std::default_random_engine randomEngine;
};


#endif //RAYTRACER_PHOTONMAP_H
