#include <chrono>
#include <vector>
#include "KdTree.h"

KdTree::KdTree(const std::vector<Object3d *> &objects) {
    std::cout << "Start building Kd-tree..." << std::endl;
    auto startTime = std::chrono::steady_clock::now();

    if (objects.empty()) {
        root = nullptr;
        return;
    }
    // Вычисляем всеобъемлющий box
    BoundingBox box(objects.front()->getBoundingBox());
    for (const Object3d *object : objects) {
        box.extend(object->getBoundingBox());
    }

    root.reset(new KdNode(box, objects));
    nodeNumber = 1;
    split(root, 0);

    auto endTime = std::chrono::steady_clock::now();
    auto workTime = std::chrono::seconds(std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count());
    std::cout << "Kd-tree has been built: "
    << nodeNumber << " nodes" << std::endl
    << "Total time " << workTime.count() / 60 << "m "
    << workTime.count() % 60 << "s" << std::endl;
}

void KdTree::split(std::unique_ptr<KdNode> &node, size_t depth) {
    // Выбрать плоскость разбиения, которая делит данный узел на два дочерних.
    Axis splitAxis;
    double splitPoint;
    // Если не нужно делить, то получили лист.
    if (!findSplitPlane(splitMethod, node, splitAxis, splitPoint)) {
        node->setIsLeaf(true);
        node->resetLeftPointer(nullptr);
        node->resetRightPointer(nullptr);
        return;
    }

    // Иначе делим box и его объекты на поддеревья найденым способом.
    node->setIsLeaf(false);
    node->setSplitAxis(splitAxis);
    node->setSplitPoint(splitPoint);

    auto leftBox = node->getBox();
    leftBox.maxCorner[splitAxis] = splitPoint;
    std::vector<Object3d *> leftObjects;

    auto rightBox = node->getBox();
    rightBox.minCorner[splitAxis] = splitPoint;
    std::vector<Object3d *> rightObjects;

    // Распределяем объекты по поддеревьям.
    for (Object3d *pObject : node->getObjects()) {
        if (pObject->isIntersectBox(leftBox)) {
            leftObjects.push_back(pObject);
        }
        if ((pObject)->isIntersectBox(rightBox)) {
            rightObjects.push_back(pObject);
        }
    }
    node->clearObjects();

    // Добавить примитивы, пересекающиеся с боксом левого узла в левый узел,
    // примитивы, пересекающиеся с боксом правого узла в правый.
    node->resetLeftPointer(new KdNode(leftBox, leftObjects));
    node->resetRightPointer(new KdNode(rightBox, rightObjects));
    nodeNumber += 2;
    split(node->getLeftSubTree(), depth + 1);
    split(node->getRightSubTree(), depth + 1);
}

double KdTree::surfaceAreaHeuristic(Axis splitAxis, double splitPoint, const BoundingBox &box,
                                    const std::vector<Object3d *> &objects) {
    auto leftBox = box;
    leftBox.maxCorner[splitAxis] = splitPoint; // todo : make separate method
    unsigned long leftObjectNumber = calculateNumberOfPrimitivesInBox(objects, leftBox);

    auto rightBox = box;
    rightBox.minCorner[splitAxis] = splitPoint;
    unsigned long rightObjectNumber = calculateNumberOfPrimitivesInBox(objects, rightBox); //todo: сделать в один проход

    return COST_EMPTY + leftObjectNumber * leftBox.surfaceArea() + rightObjectNumber * rightBox.surfaceArea();
}

unsigned long KdTree::calculateNumberOfPrimitivesInBox(const std::vector<Object3d *> &objects,
                                                       const BoundingBox &box) {
    unsigned long count = 0;
    for (const Object3d *object : objects) {
        if (object->isIntersectBox(box)) {
            count++;
        }
    }
    return count;
}

bool KdTree::findSplitPlane(SplitMethod method, std::unique_ptr<KdNode> &node, Axis &splitAxis, double &splitPoint) {
    if (node->getObjectsNumber() == 0) {
        return false;
    }
    switch (method) {
        case SPLIT_BY_REGULAR_GRID:
            return findSplitByGrid(node, splitAxis, splitPoint);
        case SPLIT_BY_BOUNDS:
            return findSplitByBounds(node, splitAxis, splitPoint);
        case SPLIT_ADAPTIVE:
            if (node->getObjectsNumber() <= getRegularGridCount()) {
                return findSplitByBounds(node, splitAxis, splitPoint);
            } else {
                return findSplitByGrid(node, splitAxis, splitPoint);
            }

        default:
            assert(false);
    }
}

bool KdTree::findSplitByBounds(std::unique_ptr<KdNode> &node, Axis &splitAxisMin, double &splitPointMin) {
    auto surroundBox = node->getBox();
    splitPointMin = surroundBox.minCorner.x;
    splitAxisMin = AXIS_X;

    double heuristicsMin = surfaceAreaHeuristic(splitAxisMin, splitPointMin, surroundBox, node->getObjects());

    // Для каждой оси и для каждого пробного положения высчитать эвристику площади и найти минимум.
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis axis = static_cast<Axis> (axisIter);
        // Т.к. границы примитивов могут и не принадлежать surroundBox, то это нужно проверять.
        double splitPointFrom = surroundBox.minCorner[axis];
        double splitPointTo = surroundBox.maxCorner[axis];
        for (Object3d *pObject : node->getObjects()) {
            auto objectBox = pObject->getBoundingBox();
            double splitPoint = objectBox.minCorner[axis];
            if (splitPoint >= splitPointFrom && splitPoint <= splitPointTo) {
                double heuristics = surfaceAreaHeuristic(axis, splitPoint, surroundBox, node->getObjects());
                if (heuristics < heuristicsMin) {
                    heuristicsMin = heuristics;
                    splitAxisMin = axis;
                    splitPointMin = splitPoint;
                }
            }
        }
    }
    // Если стоимость прослеживания дочерних узлов не будет меньше
    // чем стоимость прослеживания узла целиком, то остановится.
    return node->getObjects().size() * surroundBox.surfaceArea() > heuristicsMin;
}

bool KdTree::findSplitByGrid(std::unique_ptr<KdNode> &node, Axis &splitAxisMin, double &splitPointMin) {
    auto surroundBox = node->getBox();
    splitPointMin = surroundBox.minCorner.x;
    splitAxisMin = AXIS_X;
    double heuristicsMin = surfaceAreaHeuristic(splitAxisMin, splitPointMin, surroundBox, node->getObjects());
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis axis = static_cast<Axis> (axisIter);
        for (size_t i = 0; i < REGULAR_GRID_COUNT; ++i) {
            double splitPoint = surroundBox.minCorner[axis]
                                + i * (surroundBox.maxCorner[axis] - surroundBox.minCorner[axis])
                                  / REGULAR_GRID_COUNT;
            double heuristics = surfaceAreaHeuristic(axis, splitPoint, surroundBox, node->getObjects());
            if (heuristics < heuristicsMin) {
                heuristicsMin = heuristics;
                splitAxisMin = axis;
                splitPointMin = splitPoint;
            }
        }
    }
    // Если стоимость прослеживания дочерних узлов не будет меньше
    // чем стоимость прослеживания узла целиком, то остановится.
    return node->getObjects().size() * surroundBox.surfaceArea() > heuristicsMin;
}
