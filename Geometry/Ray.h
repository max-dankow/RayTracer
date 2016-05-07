#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "Vector3d.h"

// Представляет луч, исходящий из точки origin по вектору направления direction.
class Ray {
public:

    Ray() { }

    Ray(const Point &origin, const Vector3d &direction, float power = 1) :
            origin(origin), direction(direction.normalize()),  power(power){ }

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
        origin = origin + direction.normalize() * Geometry::PRECISION;
    }

    float getPower() const {
        return power;
    }

    Vector3d reflectRay(const Vector3d &normal) const {
        Vector3d normalizedDirection = direction.normalize();
        double cosRayNormal = Vector3d::dotProduct(normal, normalizedDirection);
        // Если луч направлен с лицевой стороны.
        if (cosRayNormal <= 0) {
            cosRayNormal = fabs(cosRayNormal);
            Vector3d reflectionDirection(((normal * cosRayNormal) + normalizedDirection) * 2 - normalizedDirection);
            assert(Geometry::areDoubleEqual(cosRayNormal, Vector3d::dotProduct(reflectionDirection, normal)));
            return reflectionDirection;
        }
        return Vector3d::nullVector();
    }

    Vector3d refractRay(const Vector3d &normal, double q) const{
        if (Geometry::areDoubleEqual(q, 0)) {
            return Vector3d(0, 0, 0);
        }
        double cosRayNormal = Vector3d::dotProduct(normal, direction.normalize());
        // Если луч направлен с изнаночной стороны объекта, то нужно использовать 1 / q.
        if (cosRayNormal > 0) {
            return refractRay(normal * -1, 1 / q);
        }
        double sinThetaSqr = q * q * (1 - cosRayNormal * cosRayNormal);
        // Полное внутреннее отражение.
        if (sinThetaSqr > 1) {
            return Vector3d(0, 0, 0);
        }
        return direction.normalize() * q - normal * (q * cosRayNormal + sqrt(1 - sinThetaSqr));
    }

    static constexpr float INSIGNIFICANT = (const float) (1. / 256.);
private:
    Point origin;
    Vector3d direction;
    float power;
};

#endif //RAYTRACER_RAY_H
