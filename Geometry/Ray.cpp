#include "Ray.h"

bool Ray::intersectPlane(const Vector3d &a, const Vector3d &b, const Point &point, Real &intersection) const {
    Vector3d normal = Vector3d::crossProduct(a, b);
    Real D = Vector3d::dotProduct(normal, point);
    return intersectPlane(normal, D, point, intersection);
}

bool Ray::intersectPlane(const Vector3d &normal, const Real D, const Point &point, Real &intersection) const {
    Real dotNormalAndDir = Vector3d::dotProduct(normal, direction);
    // Проверяем параллельны ли луч и плоскость треугольника.
    if (Geometry::areRealNumbersEqual(dotNormalAndDir, 0)) {
        return false;
    }

    // Решаем систему уравнений и находим t.
    Real t = -(Vector3d::dotProduct(normal, origin) - D) / dotNormalAndDir;
    if (t < 0) {
        return false;
    } else {
        intersection = t;
        return true;
    }
}

Vector3d Ray::refractRay(const Vector3d &normal, Real refractiveIndex) const {
    if (Geometry::areRealNumbersEqual(refractiveIndex, 0)) {
        return Vector3d::nullVector();
    }
    Real cosRayNormal = Vector3d::dotProduct(normal, direction.normalize());
    // Если луч направлен с изнаночной стороны объекта, то нужно использовать 1 / q.
    if (cosRayNormal > 0) {
        return refractRay(normal * -1, 1 / refractiveIndex);
    }
    Real sinThetaSqr = refractiveIndex * refractiveIndex * (1 - cosRayNormal * cosRayNormal);
    // Полное внутреннее отражение.
    if (sinThetaSqr > 1) {
        return Vector3d(0, 0, 0);
    }
    return direction.normalize() * refractiveIndex - normal * (refractiveIndex * cosRayNormal + sqrt(1 - sinThetaSqr));
}

Vector3d Ray::reflectRay(const Vector3d &normal) const {
    Vector3d normalizedDirection = direction.normalize();
    Real cosRayNormal = Vector3d::dotProduct(normal, normalizedDirection);
    // Если луч направлен с лицевой стороны, то отражение происходит.
    if (cosRayNormal <= 0) {
        cosRayNormal = std::abs(cosRayNormal);
        Vector3d reflectionDirection(((normal * cosRayNormal) + normalizedDirection) * 2 - normalizedDirection);
        assert(Geometry::areRealNumbersEqual(cosRayNormal, Vector3d::dotProduct(reflectionDirection, normal)));
        return reflectionDirection;
    }
    return Vector3d::nullVector();
}