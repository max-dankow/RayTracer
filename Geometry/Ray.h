#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "Vector3d.h"

// Представляет луч, исходящий из точки origin по единичному вектору направления direction.
class Ray {
public:

    Ray() { }

    Ray(const Point &origin, const Vector3d &direction) : origin(origin), direction(direction.normalize()) { }

    const Point &getOrigin() const {
        return origin;
    }

    const Vector3d &getDirection() const {
        return direction;
    }

    Point getPointAt(const double t) const {
        if (t < 0 || Geometry::areDoubleEqual(t, 0)) {
            return origin;
        } else {
            return origin + direction * t;
        }
    }

    bool intersectPlane(const Vector3d &a, const Vector3d &b, const Point &point, double &intersection) const {
        Vector3d normal = Vector3d::crossProduct(a, b);
        double D = Vector3d::dotProduct(normal, point);
        return intersectPlane(normal, D, point, intersection);
    }

    bool intersectPlane(const Vector3d &normal, const double D, const Point &point, double &intersection) const {
        double dotNormalAndDir = Vector3d::dotProduct(normal, direction);
        // Проверяем параллельны ли луч и плоскость треугольника.
        if (Geometry::areDoubleEqual(dotNormalAndDir, 0)) {
            return false;
        }

        // Решаем систему уравнений и находим t.
        double t = -(Vector3d::dotProduct(normal, origin) - D) / dotNormalAndDir;
        if (t < 0) {
            return false;
        } else {
            intersection = t;
            return true;
        }
    }

    void push() {
        origin = origin + direction.normalize() * Geometry::PRECISION   ;
    }

private:
    Point origin;
    Vector3d direction;
};

#endif //RAYTRACER_RAY_H
