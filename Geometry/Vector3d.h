#ifndef RAYTRACER_VECTOR3D_H
#define RAYTRACER_VECTOR3D_H

#include <assert.h>
#include <iostream>
#include "Geometry.h"

struct Vector3d {
    Vector3d() : x(0), y(0), z(0) { }

    Vector3d(double x, double y, double z) : x(x), y(y), z(z) { }

    Vector3d(const Vector3d &from, const Vector3d &to) {
        *this = to - from;
    }

    double &operator[](Axis axis) {
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

    Vector3d operator*(double k) const {
        return Vector3d(x * k, y * k, z * k);
    }

    Vector3d operator/(double k) const {
        return Vector3d(this->x / k, this->y / k, this->z / k);
    }

    double length() const {
        return sqrt(x * x + y * y + z * z);
    }

    double lengthSquared() const {
        return x * x + y * y + z * z;
    }

    Vector3d normalize() const {
        return *this / this->length();
    }

    // Скалярное произведение.
    static double dotProduct(const Vector3d &a, const Vector3d &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Векторное произведение.
    static Vector3d crossProduct(const Vector3d &a, const Vector3d &b) {
        return Vector3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    bool operator==(const Vector3d &other) const {
        return (Geometry::areDoubleEqual(this->x, other.x)
                && Geometry::areDoubleEqual(this->y, other.y)
                && Geometry::areDoubleEqual(this->z, other.z));
    }

    bool operator!=(const Vector3d &other) const {
        return !(*this == other);
    }

    bool totallyLessEqualThan(const Vector3d &other) const {
        return ((this->x < other.x || Geometry::areDoubleEqual(this->x, other.x))
                && (this->y < other.y || Geometry::areDoubleEqual(this->y, other.y))
                && (this->z < other.z || Geometry::areDoubleEqual(this->z, other.z)));
    }

    double x, y, z;
};

static std::istream &operator>>(std::istream &input, Vector3d &vector) {
    input >> vector.x >> vector.y >> vector.z;
    return input;
}

static std::ostream &operator<<(std::ostream &output, const Vector3d &vector) {
    output << vector.x << ' ' << vector.y << ' ' << vector.z;
    return output;
}

// typedef для разделения сущностей точки и вектора.
typedef Vector3d Point;

#endif //RAYTRACER_VECTOR3D_H