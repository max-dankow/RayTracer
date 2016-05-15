#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "Vector3d.h"

// Представляет луч, исходящий из точки viewPoint по вектору направлению direction.
class Ray {
public:

    Ray() { }

    Ray(const Point &origin, const Vector3d &direction, float power = 1) :
            origin(origin), direction(direction.normalize()),
            power(power) { }

    const Point &getOrigin() const {
        return origin;
    }

    const Vector3d &getDirection() const {
        return direction;
    }

    Point getPointAt(const Real t) const {
        if (t < 0 || Geometry::areRealNumbersEqual(t, 0)) {
            return origin;
        } else {
            return origin + direction * t;
        }
    }

    bool intersectPlane(const Vector3d &a, const Vector3d &b, const Point &point, Real &intersection) const;

    bool intersectPlane(const Vector3d &normal, const Real D, const Point &point, Real &intersection) const;

    void push() {
        origin = origin + direction.normalize() * Geometry::PRECISION;
    }

    float getPower() const {
        return power;
    }

    Vector3d reflectRay(const Vector3d &normal) const;

    Vector3d refractRay(const Vector3d &normal, Real refractiveIndex) const;

    static constexpr float INSIGNIFICANT = (const float) (1. / 256.);
private:
    Point origin;
    Vector3d direction;
    float power;
};

#endif //RAYTRACER_RAY_H
