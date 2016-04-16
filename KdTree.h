#ifndef RAYTRACER_KDTREE_H
#define RAYTRACER_KDTREE_H

#include <memory>
#include <vector>
#include "Geometry/Geometry.h"
#include "Objects/Object3d.h"

enum SplitMethod {
    // Пробные точки для эвритики разбиения берутся по границам примитивов.
            SPLIT_BY_BOUNDS,
    // Пробные точки для эвритики разбиения берутся по сетке с фиксированным шагом.
            SPLIT_BY_REGULAR_GRID,
    // Начиная с определенной глубины вместо метода SPLIT_BY_REGULAR_GRID используется метод SPLIT_BY_BOUNDS.
            SPLIT_ADAPTIVE
};

class KdNode {
public:
    KdNode() { }

    KdNode(const BoundingBox &box,
           const std::vector<Object3d *> &objects) :
            box(box), objects(objects) { }

    const BoundingBox &getBox() const {
        return box;
    }

    Axis getSplitAxis() const {
        return splitAxis;
    }

    void setSplitAxis(Axis splitAxis) {
        KdNode::splitAxis = splitAxis;
    }

    double getSplitPoint() const {
        return splitPoint;
    }

    void setSplitPoint(double splitPoint) {
        KdNode::splitPoint = splitPoint;
    }

    std::unique_ptr<KdNode> &getLeftSubTree() {
        return leftSubTree;
    }

    void resetLeftPointer(KdNode *&&node) {
        leftSubTree.reset(node);
    }

    std::unique_ptr<KdNode> &getRightSubTree() {
        return rightSubTree;
    }

    KdNode *getRightPointer() {
        return rightSubTree.get();
    }

    KdNode *getLeftPointer() {
        return leftSubTree.get();
    }

    void resetRightPointer(KdNode *&&node) {
        rightSubTree.reset(node);
    }

    std::vector<Object3d *> &getObjects() { // todo : наличие такого прямого доступа подозрительно
        return objects;
    }

    void clearObjects() {
        objects.clear();
    }

    void setObjects(std::vector<Object3d *> &&objects) {
        KdNode::objects = std::move(objects);
    }

    bool getIsLeaf() const { //todo: плохо выглядит
        return isLeaf;
    }

    void setIsLeaf(bool isLeaf) {
        KdNode::isLeaf = isLeaf;
    }

    unsigned long getObjectsNumber() const {
        return objects.size();
    }

private:
    bool isLeaf;
    BoundingBox box;
    Axis splitAxis;
    double splitPoint;
    std::unique_ptr<KdNode> leftSubTree, rightSubTree;
    // Если не лист, то хранит список объектов принадлежащий обоим поддеревьям.
    std::vector<Object3d *> objects;
};


class KdTree {
public:
    KdTree(const std::vector<Object3d *> &objects);

    KdTree(const KdTree &) = delete;

    KdTree(KdTree &&other) {
        this->root = std::move(other.root);
    }

    KdNode *const getRoot() const {
        return root.get();
    }

private:
    // Рекурсивное разбиение узла.
    void split(std::unique_ptr<KdNode> &node, size_t depth);

    double surfaceAreaHeuristic(Axis splitAxis, double splitPoint, const BoundingBox &box,
                                const std::vector<Object3d *> &objects);

    unsigned long calculateNumberOfPrimitivesInBox(const std::vector<Object3d *> &objects,
                                                   const BoundingBox &box);
    size_t getRegularGridCount() const {
        return REGULAR_GRID_COUNT;
    }

    // Ищет плоскость, разбивающую box самым выгодным способом.
    // Возвращает true, если такая плоскость существует, и false, если не нужно делить.
    bool findSplitPlane(SplitMethod method, std::unique_ptr<KdNode> &node, Axis &splitAxis, double &splitPoint);

    bool findSplitByBounds(std::unique_ptr<KdNode> &node, Axis &splitAxisMin, double &splitPointMin);
    bool findSplitByGrid(std::unique_ptr<KdNode> &node, Axis &splitAxisMin, double &splitPointMin);

    const SplitMethod splitMethod = SPLIT_ADAPTIVE;
    static const size_t REGULAR_GRID_COUNT = 32;
    const double COST_EMPTY = 0;
    size_t nodeNumber;
    std::unique_ptr<KdNode> root;
};

#endif //RAYTRACER_KDTREE_H
