#ifndef RAYTRACER_GEOMETRY_H
#define RAYTRACER_GEOMETRY_H

#include <cmath>
#include <iostream>

static const double PRECISION = 1e-6;

static bool areDoubleEqual(const double a, const double b) {
    return (fabs(a - b) < PRECISION);
}

class Vector3d {
public:
    Vector3d() { }

    Vector3d(double x, double y, double z) : x(x), y(y), z(z) { }

    Vector3d(const Vector3d &from, const Vector3d &to) {
        *this = to - from;
    }

    Vector3d operator+(Vector3d other) const {
        return Vector3d(this->x + other.x, this->y + other.y, this->z + other.z);
    }

    Vector3d operator-(Vector3d other) const {
        return Vector3d(this->x - other.x, this->y - other.y, this->z - other.z);
    }

    Vector3d operator*(double k) const {
        return Vector3d(this->x * k, this->y * k, this->z * k);
    }

    double length() const {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3d normalize() const {
        return *this * (1 / this->length());
    }

    static double dotProduct(const Vector3d &a, const Vector3d &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Векторное произведение.
    static Vector3d crossProduct(const Vector3d &a, const Vector3d &b) {
        return Vector3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    bool operator==(const Vector3d &other) const {
        return (fabs(this->x - other.x) < PRECISION
                && fabs(this->y - other.y) < PRECISION
                && fabs(this->z - other.z) < PRECISION);
    }

    bool operator!=(const Vector3d &other) const {
        return !(*this == other);
    }

    double x, y, z;
};

typedef Vector3d Point;

// Представляет луч, исходящий из точки origin по единичному вектору направления direction.
class Ray {
public:

    Ray(const Vector3d &origin, const Vector3d &direction) : origin(origin), direction(direction/*.normalize()*/) { }


    const Vector3d &getOrigin() const {
        return origin;
    }

    void setOrigin(const Vector3d &origin) {
        Ray::origin = origin;
    }

    const Vector3d &getDirection() const {
        return direction;
    }

    void setDirection(const Vector3d &direction) {
        Ray::direction = direction;
    }

private:
    Vector3d origin, direction;
};

static std::istream &operator>>(std::istream &input, Vector3d &vector) {
    double x, y, z;
    input >> x >> y >> z;
    vector.x = x;
    vector.y = y;
    vector.z = z;


    return input;
}

#endif //RAYTRACER_GEOMETRY_H
