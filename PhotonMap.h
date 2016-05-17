#ifndef RAYTRACER_PHOTONMAP_H
#define RAYTRACER_PHOTONMAP_H

#include <vector>
#include <algorithm>
#include "KdTree.h"
#include "LightSources/LightSource.h"
#include "Objects/Object3d.h"

// todo : parallel?
// todo : projection map
class PhotonMap {
public:
    PhotonMap() { }

    PhotonMap(PhotonMap &&other) :
            generalTree(std::move(other.generalTree)),
            storedPhotons(std::move(other.storedPhotons)) { }

    PhotonMap &operator=(PhotonMap &&other) {
        this->generalTree = std::move(other.generalTree);
        return *this;
    }

    PhotonMap(const std::vector<LightSource *> &lights, const KdTree &kdTree, unsigned long photonNumber) {
        // todo: time и progress bar
        std::cout << "\nBuilding Photon Map (" << photonNumber << " photons)...\n";
        double totalPower = 0;
        for (LightSource *light : lights) {
            totalPower += light->getBrightness();
        }
        // Чтобы не делить на 0, если вообще нет света, выходим.
        if (Geometry::areDoubleEqual(totalPower, 0)) {
            std::cout << "Photon Map has been built, no lights\n";
            return;
        }
        // Генерация и трассировка фотонов.
        for (LightSource *light : lights) {
            long photonsPerThisLight = long(light->getBrightness() * photonNumber / totalPower);
            for (long i = 0; i < photonsPerThisLight; ++i) {
                auto photon = light->emitPhoton();
                photon.setColor(photon.getColor() * (1000. / photonsPerThisLight));
                tracePhoton(kdTree, photon, 3);
            }
        }
        // Построение фотонного kd-дерева.
        generalTree = KdTree(
                std::vector<GeometricShape *>(storedPhotons.begin(), storedPhotons.end())); // todo: may be improve
        std::cout << "Photon Map has been built\n";
    }


    const std::vector<Photon *> &getStoredPhotons() const {
        return storedPhotons;
    }

    std::vector<Photon *> locatePhotons(const Point &testPoint, double maxDistance, size_t number) {

        struct HeapNode {

            HeapNode(double sqrDistance_, Photon *photon_) : sqrDistance(sqrDistance_), photon(photon_) { }

            double sqrDistance;
            Photon *photon;

            bool operator<(const HeapNode &other) {
                return sqrDistance < other.sqrDistance;
            }
        };

        Point point(testPoint);
        double sqrMaxDistance = maxDistance * maxDistance;
        std::vector<HeapNode> heap;
        KdNode *node = generalTree.getRoot();
        if (node == nullptr || number == 0) {
            return std::vector<Photon *>();
        }
        std::stack<KdNode *> stackNodes;
        std::stack<double> stackDistToPlane;
        double estimateSqrDistance = 0;
        bool terminate = false;
        while (!terminate) {
            if (node->isLeaf()) {
                // Пора считать.
                for (GeometricShape *shape : node->getObjects()) {
                    auto photon = dynamic_cast<Photon *> (shape);
                    double sqrDistance = Vector3d(photon->getRay().getOrigin(), point).lengthSquared();
                    if (sqrDistance < sqrMaxDistance) {
                        heap.emplace_back(sqrDistance, photon);
                        std::push_heap(heap.begin(), heap.end());
                        // Если уже набрали сколько нужно, то нужно отсекать заведомо плохие варианты.
                        if (heap.size() > number) {
                            sqrMaxDistance = heap.front().sqrDistance;
                            std::pop_heap(heap.begin(), heap.end());
                            heap.pop_back();
                        }
                    }
                }
            } else {
                double distanceToPlane = point.getAxis(node->getSplitAxis()) - node->getSplitPoint();
                if (distanceToPlane < 0) {
                    // Сначала ищем в leftBox.
                    if (distanceToPlane * distanceToPlane < sqrMaxDistance) {
                        stackNodes.push(node->getRightPtr().get());
                        stackDistToPlane.push(distanceToPlane * distanceToPlane);
                    }
                    node = node->getLeftPtr().get();
                    continue;
                } else {
                    // Сначала ищем в rightBox.
                    if (distanceToPlane * distanceToPlane < sqrMaxDistance) {
                        stackNodes.push(node->getLeftPtr().get());
                        stackDistToPlane.push(distanceToPlane * distanceToPlane);
                    }
                    node = node->getRightPtr().get();
                    continue;
                }
            }

            // Если ничего не найдено то вернуться на уровень выше.
            do {
                if (stackNodes.empty()) {
                    terminate = true;
                    break;
                }
                node = stackNodes.top();
                stackNodes.pop();

                // Проверяем, есть ли еще смысл искать в сопряженном поддереве, так как рассотояние
                // до любой точки в нем не меньше расстояния до плоскости, а значит если оно уже больше
                // критического, то нет смысла рассматривать это поддерево.
                estimateSqrDistance = stackDistToPlane.top();
                stackDistToPlane.pop();
            } while (estimateSqrDistance > sqrMaxDistance);
        }
        std::vector<Photon *> result;
        result.reserve(heap.size());
        for (HeapNode &heapNode : heap) {
            result.push_back(heapNode.photon);
        }
        return result;
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
        if (randomVariable < obstacleMaterial->reflectance) {
            // Решается, что луч отразился.
            auto reflectedDirection = photon.getRay().reflectRay(obstacle->getNormal(hitPoint));
            Ray reflectedRay(hitPoint, reflectedDirection);
            reflectedRay.push();
            Photon reflectedPhoton(reflectedRay, photon.getColor());
            tracePhoton(objects, reflectedPhoton, restDepth - 1);
        } else {
            if (randomVariable < obstacleMaterial->reflectance + obstacleMaterial->transparency) {
                // Решается, что луч преломился.
                auto refractionDirection = photon.getRay().refractRay(obstacle->getNormal(hitPoint),
                                                                      obstacleMaterial->refractiveIndex);
                if (!Vector3d::isNullVector(refractionDirection)) {
                    Ray refractedRay(hitPoint, refractionDirection);
                    refractedRay.push();
                    Photon refractedPhoton(refractedRay, photon.getColor() * obstacle->getColorAt(hitPoint));
                    tracePhoton(objects, refractedPhoton, restDepth - 1);
                }
            } else {
                storedPhotons.push_back(new Photon(hitPoint, photon.getDirection(), photon.getColor()));
            }
        }

    }

    KdTree generalTree;
    std::vector<Photon *> storedPhotons;
    std::default_random_engine randomEngine;
};


#endif //RAYTRACER_PHOTONMAP_H
