#ifndef RAYTRACER_BOUNDINGBOX_H
#define RAYTRACER_BOUNDINGBOX_H

#include <algorithm>
#include "Vector3d.h"
#include "Ray.h"

// Представляет параллелограм со сторонами параллельными осям, обрамляющий некоторые фигуры в пространстве.
// Характеризуется точкой с минимальными координатами minCorner и точкой с максимальными координатами maxCorner.
struct BoundingBox {

    BoundingBox() { }

    BoundingBox(const Point &minCorner, const Point &maxCorner) :
            minCorner(minCorner), maxCorner(maxCorner) { }

    // Конструирует BoundingBox обрамляющий указанные точки.
    BoundingBox(const std::vector<Point> &points);

    // Расширяет BoundingBox так, чтобы он обрамлял исходный и otherBox.
    void extend(const BoundingBox &otherBox);

    Real surfaceArea() const;

    bool containsPoint(const Point &point) const {
        return point.totallyLessEqualThan(maxCorner) && minCorner.totallyLessEqualThan(point);
    }

    bool intersectBox(const BoundingBox &other) const {
        return this->minCorner.totallyLessEqualThan(other.maxCorner)
               && other.minCorner.totallyLessEqualThan(this->maxCorner);
    }

    Real intersectRay(const Ray &ray) const;

    Point minCorner, maxCorner;

private:
    // Возвращает точку с координатами, минимальными по каждой оси.
    static Point getMinPoint(const Point &first, const Point &second) {
        return Point(std::min(first.x, second.x),
                     std::min(first.y, second.y),
                     std::min(first.z, second.z));
    }

    // Возвращает точку с координатами, максимальными по каждой оси.
    static Point getMaxPoint(const Point &first, const Point &second) {
        return Point(std::max(first.x, second.x),
                     std::max(first.y, second.y),
                     std::max(first.z, second.z));
    }
};

#endif //RAYTRACER_BOUNDINGBOX_H
