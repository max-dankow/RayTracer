#ifndef RAYTRACER_VECTOR3D_H
#define RAYTRACER_VECTOR3D_H

#include <assert.h>
#include <iostream>
#include "Geometry.h"

struct Vector3d {
    Vector3d() : x(0), y(0), z(0) { }

    Vector3d(Real x, Real y, Real z) : x(x), y(y), z(z) { }

    Vector3d(const Vector3d &from, const Vector3d &to) {
        *this = to - from;
    }

    void setAxis(Axis axis, Real newValue) {
        switch (axis) {
            case AXIS_X:
                x = newValue;
            case AXIS_Y:
                y = newValue;
            case AXIS_Z:
                z = newValue;
            default:
                assert(false);
        }
    }

    Real getAxis(Axis axis) const {
        switch (axis) {
            case AXIS_X:
                return x;
            case AXIS_Y:
                return y;
            case AXIS_Z:
                return z;
            default:
                assert(false);
        }
    }

    Vector3d operator+(const Vector3d &other) const {
        return Vector3d(this->x + other.x, this->y + other.y, this->z + other.z);
    }

    Vector3d operator-(const Vector3d &other) const {
        return Vector3d(this->x - other.x, this->y - other.y, this->z - other.z);
    }

    Vector3d operator*(Real k) const {
        return Vector3d(x * k, y * k, z * k);
    }

    Vector3d operator/(Real k) const {
        return Vector3d(this->x / k, this->y / k, this->z / k);
    }

    Real length() const {
        return (Real) sqrt(x * x + y * y + z * z);
    }

    Real lengthSquared() const {
        return x * x + y * y + z * z;
    }

    Vector3d normalize() const {
        return *this / this->length();
    }

    // Скалярное произведение.
    static Real dotProduct(const Vector3d &a, const Vector3d &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Векторное произведение.
    static Vector3d crossProduct(const Vector3d &a, const Vector3d &b) {
        return Vector3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    bool operator==(const Vector3d &other) const {
        return (Geometry::areRealNumbersEqual(this->x, other.x)
                && Geometry::areRealNumbersEqual(this->y, other.y)
                && Geometry::areRealNumbersEqual(this->z, other.z));
    }

    bool operator!=(const Vector3d &other) const {
        return !(*this == other);
    }

    static Vector3d nullVector() {
        return Vector3d(0, 0, 0);
    }

    static bool isNullVector(const Vector3d &vector) {
        return vector == Vector3d(0, 0, 0);
    }

    bool totallyLessEqualThan(const Vector3d &other) const {
        return ((this->x < other.x || Geometry::areRealNumbersEqual(this->x, other.x))
                && (this->y < other.y || Geometry::areRealNumbersEqual(this->y, other.y))
                && (this->z < other.z || Geometry::areRealNumbersEqual(this->z, other.z)));
    }

    friend std::istream &operator>>(std::istream &input, Vector3d &vector) {
        input >> vector.x >> vector.y >> vector.z;
        return input;
    }

    friend std::ostream &operator<<(std::ostream &output, const Vector3d &vector) {
        output << vector.x << ' ' << vector.y << ' ' << vector.z;
        return output;
    }

    bool belongsToTriangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, const Vector3d &normal) const {
        Vector3d ab(a, b), bc(b, c), ca(c, a);
        Vector3d ap(a, *this), bp(b, *this), cp(c, *this);
        return Vector3d::dotProduct(normal, Vector3d::crossProduct(ab, ap)) >= 0
               && Vector3d::dotProduct(normal, Vector3d::crossProduct(bc, bp)) >= 0
               && Vector3d::dotProduct(normal, Vector3d::crossProduct(ca, cp)) >= 0;
    }

    Real x, y, z;
};

// typedef для разделения сущностей точки и вектора.
typedef Vector3d Point;

#endif //RAYTRACER_VECTOR3D_H
