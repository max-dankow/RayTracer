#ifndef RAYTRACER_KDTREE_H
#define RAYTRACER_KDTREE_H

#include <memory>
#include <vector>
#include "Geometry.h"
#include "Objects/Object3d.h"

class KdNode {
public:
    KdNode() { }

    KdNode(const BoundingBox &box, KdNode* parent,
           std::vector<Object3d*> &&objects) :
            box(box), parent(parent), objects(std::move(objects)) { }

    const BoundingBox &getBox() const {
        return box;
    }

    void setBox(const BoundingBox &box) {
        KdNode::box = box;
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

    void resetRightPointer(KdNode* &&node) {
        rightSubTree.reset(node);
    }

    std::vector<Object3d*> &getObjects() { // todo : наличие такого прямого доступа подозрительно
        return objects;
    }

    void setObjects(std::vector<Object3d*> &&objects) {
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
    // Флаг, является ли узел листом.
    bool isLeaf;
    BoundingBox box;
    Axis splitAxis;
    double splitPoint;
    std::unique_ptr<KdNode> leftSubTree, rightSubTree;
    KdNode* parent;  // Ресурсом владет другой указатель
    // Если не лист, то хранит список объектов принадлежащий обоим поддеревьям.
    std::vector<Object3d*> objects;
};


class KdTree {
public:
    KdTree(std::vector<Object3d*> &&objects);

    KdTree(const KdTree &) = delete;

    std::unique_ptr<KdNode> root;
private:
    static const size_t MAX_DEPTH = 10;
    static const size_t MIN_OBJECTS_PER_LIST = 1;
    static const size_t REGULAR_GRID_COUNT = 10;
    const double COST_EMPTY = 0;

    void split(std::unique_ptr<KdNode> &node, size_t depth);

    double surfaceAreaHeuristic(Axis splitAxis, double splitPoint, const BoundingBox &box,
                                    const std::vector<Object3d*> &objects);

    unsigned long calculateNumberOfPrimitivesInBox(const std::vector<Object3d*> &objects,
                                                   const BoundingBox &box);

};

#endif //RAYTRACER_KDTREE_H
