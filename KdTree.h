#ifndef RAYTRACER_KDTREE_H
#define RAYTRACER_KDTREE_H

#include <memory>
#include <vector>
#include "Geometry.h"
#include "Objects/Object3d.h"

enum Axis {
    AXIS_X, AXIS_Y, AXIS_Z
};

class KdNode {
public:
    KdNode() { }


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

    double getSplitCoordinate() const {
        return splitCoordinate;
    }

    void setSplitCoordinate(double splitCoordinate) {
        KdNode::splitCoordinate = splitCoordinate;
    }
//
//    const std::unique_ptr<KdNode> &getLeftSubTree() const {
//        return leftSubTree;
//    }

//    void setLeftSubTree(const std::unique_ptr<KdNode> &leftSubTree) {
//        KdNode::leftSubTree = leftSubTree;
//    }
//
//    const std::unique_ptr<KdNode> &getRightSubTree() const {
//        return rightSubTree;
//    }

//    void setRightSubTree(const std::unique_ptr<KdNode> &rightSubTree) {
//        KdNode::rightSubTree = rightSubTree;
//    }
//
//    const std::vector<std::unique_ptr<Object3d>> &getObjects() const {
//        return objects;
//    }

    void setObjects(std::vector<std::unique_ptr<Object3d>> &&objects) {
        KdNode::objects = std::move(objects);
    }

private:
    BoundingBox box;
    Axis splitAxis;
    double splitCoordinate;
    std::unique_ptr<KdNode> leftSubTree, rightSubTree;
    std::vector<std::unique_ptr<Object3d> > objects;
};


class KdTree {
public:
    KdTree(std::vector<std::unique_ptr<Object3d> > &&objects);
    KdTree(const KdTree&) = delete;
    std::unique_ptr<KdNode> root;
private:
    void split(KdNode &node, std::vector<Object3d> &&objects);
    double surfaceAreaHeuristic(const std::vector<std::unique_ptr<Object3d> > &objects,
                                Axis splitAxis, double splitPoint);
    long calculateNumeberOfPrimitivesInBox(const std::vector<std::unique_ptr<Object3d> > &objects,
                                           const BoundingBox &box);
};

#endif //RAYTRACER_KDTREE_H
