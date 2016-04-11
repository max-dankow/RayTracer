#include "KdTree.h"

KdTree::KdTree(std::vector<std::unique_ptr<Object3d> > &&objects) {
    if (objects.empty()) {
        root = nullptr;
        return;
    }
    // Вычисляем всеобъемлющий box
    BoundingBox box(objects.front()->getBoundingBox());
    for (const std::unique_ptr<Object3d> &object : objects) {
        box.extend(object->getBoundingBox());
    }
    root.reset(new KdNode());
    root->setBox(box);
    root->setObjects(std::move(objects));
    // Split(root)
}

void KdTree::split(KdNode &node, std::vector<Object3d> &&objects) {
    // Если сработал критерий остановки - вернуться
    // Выбрать плоскость разбиения, которая делит данный узел на два дочерних.
        // Для каждой оси и для каждого пробного положения высчитать эвристику площади
        // выбрать минимум
    // Добавить примитивы, пересекающиеся с боксом левого узла в левый узел, примитивы, пересекающиеся с боксом правого узла в правый.
    // возможно дублируя объекты
    // Split(Left); Split(right);
}

double KdTree::surfaceAreaHeuristic(const std::vector<std::unique_ptr<Object3d> > &objects, Axis splitAxis,
                                    double splitPoint) {
    // Рассчитать N(left)
    // Рассчитать N(right)
    return 0;
}

long KdTree::calculateNumeberOfPrimitivesInBox(const std::vector<std::unique_ptr<Object3d> > &objects,
                                               const BoundingBox &box) {
    return 0;
}







