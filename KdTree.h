#ifndef RAYTRACER_KDTREE_H
#define RAYTRACER_KDTREE_H

#include <memory>
#include <vector>
#include <stack>
#include "Geometry/Geometry.h"
#include "Geometry/GeometricShape.h"

using std::unique_ptr;

enum SplitMethod {
    // Пробные точки для эвритики разбиения берутся по границам примитивов.
            SPLIT_BY_BOUNDS,
    // Пробные точки для эвритики разбиения берутся по сетке с фиксированным шагом.
            SPLIT_BY_REGULAR_GRID,
    SPLIT_BY_GRID_FAST,
    // Начиная с определенной глубины вместо метода SPLIT_BY_REGULAR_GRID используется метод SPLIT_BY_BOUNDS.
            SPLIT_ADAPTIVE
};

class KdNode {
public:
    KdNode() { }

    KdNode(const BoundingBox &box,
           const std::vector<GeometricShape *> &objects) :
            box(box), objects(objects) { }

    KdNode(const BoundingBox &box,
           std::vector<GeometricShape *> &&objects) :
            box(box), objects(std::move(objects)) { }

    const BoundingBox &getBox() const {
        return box;
    }

    Axis getSplitAxis() const {
        return splitAxis;
    }

    void setSplitAxis(Axis splitAxis) {
        this->splitAxis = splitAxis;
    }

    float getSplitPoint() const {
        return splitPoint;
    }

    void setSplitPoint(float splitPoint) {
        KdNode::splitPoint = splitPoint;
    }

    void resetRightPointer(KdNode *&&node) {
        rightPtr.reset(node);
    }

    void resetLeftPointer(KdNode *&&node) {
        leftPtr.reset(node);
    }

    unique_ptr<KdNode> &getRightPtr() {
        return rightPtr;
    }

    unique_ptr<KdNode> &getLeftPtr() {
        return leftPtr;
    }

    const std::vector<GeometricShape *> &getObjects() const {
        return objects;
    }

    void clearObjects() {
        objects.clear();
    }

    bool isLeaf() const {
        assert(leftPtr != nullptr || rightPtr == nullptr);
        return leftPtr == nullptr;
    }

    size_t getObjectsNumber() const {
        return objects.size();
    }

private:
    BoundingBox box;
    // Ось, ортогональной которой проходит разделяющая плоскость.
    Axis splitAxis;
    // Абсолютная координата по splitAxis.
    float splitPoint;
    // Указатели на поддеревья.
    unique_ptr<KdNode> leftPtr, rightPtr;
    std::vector<GeometricShape *> objects;
};


class KdTree {
public:
    KdTree() {
        root.reset(nullptr);
    }

    KdTree(const std::vector<GeometricShape *> &objects);

    KdTree(const KdTree &) = delete;

    KdTree(KdTree &&other) {
        this->root = std::move(other.root);
        other.root.reset(nullptr);
    }

    KdTree &operator=(KdTree &&other) {
        this->root = std::move(other.root);
        return *this;
    }

    KdNode *const getRoot() const {
        return root.get();
    }

    GeometricShape *findObstacle(const Ray &ray, Point &hitPoint) const;

private:
    // Рекурсивное разбиение узла.
    void split(unique_ptr<KdNode> &node);

    // Подсчет эвристики площади поверхности sah.
    Real surfaceAreaHeuristic(Axis splitAxis, Real splitPoint, const BoundingBox &box,
                              const std::vector<GeometricShape *> &objects);

    unsigned long countPrimitivesInBox(const std::vector<GeometricShape *> &objects,
                                       const BoundingBox &box);

    size_t getRegularGridCount() const {
        return REGULAR_GRID_COUNT;
    }

    // Ищет плоскость, разбивающую box самым выгодным способом.
    // Возвращает true, если такая плоскость существует, и false, если не нужно делить.
    bool findSplitPlane(SplitMethod method, unique_ptr<KdNode> &node, Axis &splitAxis, Real &splitPoint);

    bool findSplitByBounds(unique_ptr<KdNode> &node, Axis &splitAxisMin, Real &splitPointMin);

    bool findSplitByGrid(unique_ptr<KdNode> &node, Axis &splitAxisMin, Real &splitPointMin);

    bool findSplitByGridFast(unique_ptr<KdNode> &node, Axis &splitAxisMin, Real &splitPointMin);

    GeometricShape *checkIntersection(const Ray &ray,
                                      const std::vector<GeometricShape *> &objectList,
                                      Point &intersection) const;

    const SplitMethod splitMethod = SPLIT_ADAPTIVE;
    static const size_t REGULAR_GRID_COUNT = 32;
    const Real COST_EMPTY = 0.1;
    size_t nodeCount;
    unique_ptr<KdNode> root;
};

#endif //RAYTRACER_KDTREE_H
