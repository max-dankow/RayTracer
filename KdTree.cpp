#include <chrono>
#include "KdTree.h"

KdTree::KdTree(std::vector<Object3d*> &&objects) {
    std::cout << "Start building Kd-tree..." << std::endl;
    auto startTime = std::chrono::steady_clock::now();

    if (objects.empty()) {
        root = nullptr;
        return;
    }
    // Вычисляем всеобъемлющий box
    BoundingBox box(objects.front()->getBoundingBox());
    for (const Object3d* object : objects) {
        box.extend(object->getBoundingBox());
    }

    std::cout << "Root bounding box:\n"
        << box.minCorner << '\n' << box.maxCorner << '\n';

    root.reset(new KdNode(box, nullptr, std::move(objects)));
    split(root, 0);

    auto endTime = std::chrono::steady_clock::now();
    auto workTime = std::chrono::seconds(std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count());
    std::cout << "Kd-tree has been built" << std::endl
        << "Total time " << workTime.count() / 60 << "m "
        << workTime.count() % 60 << "s" << std::endl;
}

void KdTree::split(std::unique_ptr<KdNode> &node, size_t depth) {
    auto surroundBox = node->getBox();
    // Выбрать плоскость разбиения, которая делит данный узел на два дочерних.
    // Для каждой оси и для каждого пробного положения высчитать эвристику площади и найти минимум.
    double splitPointMin = node->getBox().minCorner.x;
    Axis splitAxisMin = AXIS_X;
    double heuristicsMin = surfaceAreaHeuristic(splitAxisMin, splitPointMin, surroundBox, node->getObjects());
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis axis = static_cast<Axis> (axisIter);
        // Фиксированное число пробных значений - regular grid.
        for (size_t i = 0; i < REGULAR_GRID_COUNT; ++i) {
            double splitPoint = surroundBox.minCorner[axis]
                    + i * (surroundBox.maxCorner[axis] - surroundBox.minCorner[axis]) / REGULAR_GRID_COUNT;
            double heuristics = surfaceAreaHeuristic(axis, splitPoint, surroundBox, node->getObjects());
//            std::cout << splitPoint << " sah = " << heuristics << '\n';
            if (heuristics < heuristicsMin) {
                heuristicsMin = heuristics;
                splitAxisMin = axis;
                splitPointMin = splitPoint;
            }
        }
    }
    std::cout << splitAxisMin << ' ' << splitPointMin << '\n';

    // Стоимость прослеживания дочерних узлов будет не меньше
    // чем стоимость простлеживания узла целиком, то остановится.
    if (node->getObjects().size() * surroundBox.surfaceArea() <= heuristicsMin) {
        node->setIsLeaf(true);
        node->resetLeftPointer(nullptr);
        node->resetRightPointer(nullptr);
        return;
    }
    node->setIsLeaf(false);
    node->setSplitAxis(splitAxisMin);
    node->setSplitPoint(splitPointMin);

    auto leftBox = node->getBox();
    leftBox.maxCorner[splitAxisMin] = splitPointMin;
    std::vector<Object3d*> leftObjects;

    // Распределяем объекты по поддеревьям.
    auto rightBox = node->getBox();
    rightBox.minCorner[splitAxisMin] = splitPointMin;
    std::vector<Object3d*> rightObjects;

    // Объекты пересекающие оба box'a будут храниться у родителя.
    std::vector<Object3d*> parentObjects;

    for (auto iter = node->getObjects().begin(); iter < node->getObjects().end(); ++iter) {
        bool belongsToLeft = (*iter)->isIntersectBox(leftBox);
        bool belongsToRight = (*iter)->isIntersectBox(rightBox);
        if (belongsToLeft && belongsToRight) {
            parentObjects.push_back(std::move(*iter));
            continue;
        }
        if (belongsToLeft) {
            leftObjects.push_back(std::move(*iter));
        }
        if (belongsToRight) {
            rightObjects.push_back(std::move(*iter));
        }
    }

    // Добавить примитивы, пересекающиеся с боксом левого узла в левый узел, примитивы, пересекающиеся с боксом правого узла в правый.
    node->resetLeftPointer(new KdNode(leftBox, node.get(), std::move(leftObjects)));
    node->resetRightPointer(new KdNode(rightBox, node.get(), std::move(rightObjects)));
    node->setObjects(std::move(parentObjects));
    split(node->getLeftSubTree(), depth + 1);
    split(node->getRightSubTree(), depth + 1);
}

double KdTree::surfaceAreaHeuristic(Axis splitAxis, double splitPoint, const BoundingBox &box,
                                    const std::vector<Object3d*> &objects) {
    auto leftBox = box;
    leftBox.maxCorner[splitAxis] = splitPoint;
    unsigned long leftObjectNumber = calculateNumberOfPrimitivesInBox(objects, leftBox);

    auto rightBox = box;
    rightBox.minCorner[splitAxis] = splitPoint;
    unsigned long rightObjectNumber = calculateNumberOfPrimitivesInBox(objects, rightBox); //todo: сделать в один проход

    return COST_EMPTY + leftObjectNumber * leftBox.surfaceArea() + rightObjectNumber * rightBox.surfaceArea();
}

unsigned long KdTree::calculateNumberOfPrimitivesInBox(const std::vector<Object3d*> &objects,
                                                       const BoundingBox &box) {
    unsigned long count = 0;
    for (const Object3d* object : objects) {
        if (object->isIntersectBox(box)) {
            count++;
        }
    }
    return count;
}







