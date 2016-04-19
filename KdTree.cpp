#include <chrono>
#include <vector>
#include "KdTree.h"

KdTree::KdTree(const std::vector<Object3d *> &objects) {
    std::cout << "Building Kd-tree..." << std::endl;
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
        if (pObject->getBoundingBox().intersectBox(leftBox)) {
            leftObjects.push_back(pObject);
        }
        if ((pObject)->getBoundingBox().intersectBox(rightBox)) {
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
        if (object->getBoundingBox().intersectBox(box)) {
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
        case SPLIT_BY_BOUNDS:
            return findSplitByBounds(node, splitAxis, splitPoint);
        case SPLIT_BY_REGULAR_GRID:
            return findSplitByGrid(node, splitAxis, splitPoint);
        case SPLIT_BY_GRID_FAST:
            return findSplitByGridFast(node, splitAxis, splitPoint);
        case SPLIT_ADAPTIVE:
            if (node->getObjectsNumber() <= getRegularGridCount()) {
                return findSplitByBounds(node, splitAxis, splitPoint);
            } else {
                return findSplitByGridFast(node, splitAxis, splitPoint);
            }

        default:
            assert(false);
    }
}

bool KdTree::findSplitByBounds(std::unique_ptr<KdNode> &node, Axis &splitAxisMin, double &splitPointMin) {
    auto surroundBox = node->getBox();
    splitPointMin = surroundBox.minCorner.x;
    splitAxisMin = AXIS_X;

    double heuristicMin = surfaceAreaHeuristic(splitAxisMin, splitPointMin, surroundBox, node->getObjects());

    // Для каждой оси и для каждого пробного положения высчитать эвристику площади и найти минимум.
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis axis = static_cast<Axis> (axisIter);
        // Т.к. границы примитивов могут и не принадлежать surroundBox, то это нужно проверять.
        double splitPointFrom = surroundBox.minCorner[axis];
        double splitPointTo = surroundBox.maxCorner[axis];
        for (Object3d *object : node->getObjects()) {
            auto objectBox = object->getBoundingBox();
            double splitPoint = objectBox.minCorner[axis];
            if (splitPoint >= splitPointFrom && splitPoint <= splitPointTo) {
                double heuristic = surfaceAreaHeuristic(axis, splitPoint, surroundBox, node->getObjects());
                if (heuristic < heuristicMin) {
                    heuristicMin = heuristic;
                    splitAxisMin = axis;
                    splitPointMin = splitPoint;
                }
            }
        }
    }
    // Если стоимость прослеживания дочерних узлов не будет меньше
    // чем стоимость прослеживания узла целиком, то остановится.
    return node->getObjects().size() * surroundBox.surfaceArea() > heuristicMin;
}

bool KdTree::findSplitByGrid(std::unique_ptr<KdNode> &node, Axis &splitAxisMin, double &splitPointMin) {
    auto surroundBox = node->getBox();
    splitPointMin = surroundBox.minCorner.x;
    splitAxisMin = AXIS_X;
    double heuristicMin = node->getObjects().size() * surroundBox.surfaceArea();
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis axis = static_cast<Axis> (axisIter);
        for (size_t i = 0; i < REGULAR_GRID_COUNT; ++i) {
            double splitPoint = surroundBox.minCorner[axis]
                                + i * (surroundBox.maxCorner[axis] - surroundBox.minCorner[axis])
                                  / REGULAR_GRID_COUNT;
            double heuristic = surfaceAreaHeuristic(axis, splitPoint, surroundBox, node->getObjects());
            if (heuristic < heuristicMin) {
                heuristicMin = heuristic;
                splitAxisMin = axis;
                splitPointMin = splitPoint;
            }
        }
    }
    // Если стоимость прослеживания дочерних узлов не будет меньше
    // чем стоимость прослеживания узла целиком, то остановится.
    return node->getObjects().size() * surroundBox.surfaceArea() > heuristicMin;
}

bool KdTree::findSplitByGridFast(std::unique_ptr<KdNode> &node, Axis &splitAxisMin, double &splitPointMin) {
    auto surroundBox = node->getBox();
    splitPointMin = surroundBox.minCorner.x;
    splitAxisMin = AXIS_X;
    size_t gridCount = getRegularGridCount();
    double heuristicMin = node->getObjects().size() * surroundBox.surfaceArea();
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis axis = static_cast<Axis> (axisIter);
        double splitPointFrom = surroundBox.minCorner[axis];
        double splitPointTo = surroundBox.maxCorner[axis];
        double gridStep = (splitPointTo - splitPointFrom) / gridCount;

        // Сколько примитивов начинаются в соответсвующей корзинке.
        std::vector<size_t> startsHereCount(gridCount, 0);
        // Аналогично сколько заканчиваются.
        std::vector<size_t> endsHereCount(gridCount, 0);

        // Заполняем эти данные.
        for (Object3d *object : node->getObjects()) {
            auto objectBox = object->getBoundingBox();
            long binIndex;

            // startsHereCount
            double d = (objectBox.minCorner[axis] - splitPointFrom) / gridStep;
            // Если начало в плоскости разреза, то учесть его нужно уже в предыдущей корзине.
            if (Geometry::areDoubleEqual(d - std::round(d), 0)) {
                binIndex = (long) std::round(d) - 1;
            } else {
                binIndex = (long) std::floor(d);
            }
            // Если начало до splitPointFrom
            if (binIndex < 0) {
                binIndex = 0;
            }
            startsHereCount[binIndex]++;

            // endsHereCount
            d = (objectBox.maxCorner[axis] - splitPointFrom) / gridStep;
            // Если начало в плоскости разреза, то учесть его нужно уже в предыдущей корзине.
            if (Geometry::areDoubleEqual(d - std::round(d), 0)) {
                binIndex = (long) std::round(d);
            } else {
                binIndex = (long) std::floor(d);
            }
            // Если начало после splitPointTo
            if (binIndex > gridCount - 1) {
                binIndex = gridCount - 1;
            }
            endsHereCount[binIndex]++;
        }

        size_t leftObjectNumber = 0;
        size_t rightObjectNumber = std::accumulate(endsHereCount.begin(), endsHereCount.end(), size_t(0));

        // Считаем эвристику в точках сетки.
        for (size_t i = 1; i < gridCount; ++i) {
            double splitPoint = splitPointFrom + i * gridStep;
            auto leftBox = surroundBox;
            leftBox.maxCorner[axis] = splitPoint;
            leftObjectNumber += startsHereCount[i - 1];

            auto rightBox = surroundBox;
            rightBox.minCorner[axis] = splitPoint;
//            assert(rightObjectNumber >= endsHereCount[i]);//todo remove after debug
            rightObjectNumber -= endsHereCount[i - 1];

            double heuristic = COST_EMPTY + leftObjectNumber * leftBox.surfaceArea() + rightObjectNumber * rightBox.surfaceArea();
            if (heuristic < heuristicMin) {
                heuristicMin = heuristic;
                splitAxisMin = axis;
                splitPointMin = splitPoint;
            }
        }
    }
    // Если стоимость прослеживания дочерних узлов не будет меньше
    // чем стоимость прослеживания узла целиком, то остановится.
    return node->getObjects().size() * surroundBox.surfaceArea() > heuristicMin;
}