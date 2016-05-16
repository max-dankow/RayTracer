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

    // Переберем все плоскости и найдем для всех пересечение с лучом.
    for (int axisIter = 0; axisIter < 3; ++axisIter) {
        Axis currentAxis = static_cast<Axis> (axisIter);
        Axis nextAxis = Geometry::nextAxis(currentAxis);

        auto currentSide = Vector3d::nullVector();
        currentSide.setAxis(currentAxis, maxCorner.getAxis(currentAxis) - minCorner.getAxis(currentAxis));

        auto nextSide = Vector3d::nullVector();
        nextSide.setAxis(nextAxis, maxCorner.getAxis(nextAxis) - minCorner.getAxis(nextAxis));

        if (ray.intersectPlane(currentSide, nextSide, minCorner, t)) {
            // Если точка пересечения с плоскостью лежит в боксе, то обновим минимум.
            if (containsPoint(ray.getPointAt(t))) {
                t_min = std::min(t, t_min);
            }
        }
        if (ray.intersectPlane(currentSide * -1, nextSide * -1, maxCorner, t)) {
            // Если точка пересечения с плоскостью лежит в боксе, то обновим минимум.
            if (containsPoint(ray.getPointAt(t))) {
                t_min = std::min(t, t_min);
            }
        }
    }
    return t_min;
}