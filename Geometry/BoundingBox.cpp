#include <vector>
#include "BoundingBox.h"

BoundingBox::BoundingBox(const std::vector<Point> &points) {
    if (points.empty()) {
        return;
    }
    minCorner = points.front();
    maxCorner = points.front();
    for (Point point : points) {
        minCorner = getMinPoint(minCorner, point);
        maxCorner = getMaxPoint(maxCorner, point);
    }
}

void BoundingBox::extend(const BoundingBox &otherBox) {
    minCorner = getMinPoint(minCorner, otherBox.minCorner);
    maxCorner = getMaxPoint(maxCorner, otherBox.maxCorner);
}

double BoundingBox::surfaceArea() const {
    double area = 0;
    area += fabs((maxCorner.x - minCorner.x) * (maxCorner.y - minCorner.y));
    area += fabs((maxCorner.y - minCorner.y) * (maxCorner.z - minCorner.z));
    area += fabs((maxCorner.z - minCorner.z) * (maxCorner.x - minCorner.x));
    return area * 2;
}

double BoundingBox::intersectRay(const Ray &ray) const {
    // Если начало луча уже в box, то это и есть точка пересечения.
    if (containsPoint(ray.getOrigin())) {
        return 0;
    }
    double t, t_min = Geometry::INFINITY_DOUBLE;
    auto minCornerLocal = minCorner;
    auto maxCornerLocal = maxCorner;
    //todo: убрать дублирующийся код
    // Переберем все плоскости и найдем для всех пересечение с лучом.
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis axis1 = static_cast<Axis> (axisIter);
        Axis axis2 = Geometry::nextAxis(axis1);
        Vector3d v1(0, 0, 0);
        v1[axis1] = maxCornerLocal[axis1] - minCornerLocal[axis1];
        Vector3d v2(0, 0, 0);

        v2[axis2] = maxCornerLocal[axis2] - minCornerLocal[axis2];
        if (ray.intersectPlane(v1, v2, minCornerLocal, t)) {
            // Если точка пересечения с плоскостью лежит в боксе, то обновим минимум.
            if (containsPoint(ray.getPointAt(t))) {
                t_min = std::min(t, t_min);
            }
        }
    }
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis axis1 = static_cast<Axis> (axisIter);
        Axis axis2 = Geometry::nextAxis(axis1);
        Vector3d v1(0, 0, 0);
        v1[axis1] = minCornerLocal[axis1] - maxCornerLocal[axis1];
        Vector3d v2(0, 0, 0);
        v2[axis2] = minCornerLocal[axis2] - maxCornerLocal[axis2];
        if (ray.intersectPlane(v1, v2, maxCornerLocal, t)) {
            // Если точка пересечения с плоскостью лежит в боксе, то обновим минимум.
            if (containsPoint(ray.getPointAt(t))) {
                t_min = std::min(t, t_min);
            }
        }
    }
    return t_min;
}