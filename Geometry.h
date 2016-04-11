#ifndef RAYTRACER_GEOMETRY_H
#define RAYTRACER_GEOMETRY_H

#include <cmath>
#include <vector>
#include <assert.h>
#include <iostream>

static const double PRECISION = 1e-6;

static bool areDoubleEqual(const double a, const double b) {
    return (fabs(a - b) < PRECISION);
}

enum Axis {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2
};

class Vector3d {
public:
    Vector3d() { }

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

    Vector3d operator+(Vector3d other) const {
        return Vector3d(this->x + other.x, this->y + other.y, this->z + other.z);
    }

    Vector3d operator-(Vector3d other) const {
        return Vector3d(this->x - other.x, this->y - other.y, this->z - other.z);
    }

    Vector3d operator*(double k) const {
        return Vector3d(this->x * k, this->y * k, this->z * k);
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
    inline static double dotProduct(const Vector3d &a, const Vector3d &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Векторное произведение.
    static Vector3d crossProduct(const Vector3d &a, const Vector3d &b) {
        return Vector3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    bool operator==(const Vector3d &other) const {
        return (areDoubleEqual(this->x, other.x)
                && areDoubleEqual(this->y, other.y)
                && areDoubleEqual(this->z, other.z));
    }

    bool operator!=(const Vector3d &other) const {
        return !(*this == other);
    }

    bool totallyLessEqualThan(const Vector3d &other) const {
        return (this->x <= other.x && this->y <= other.y && this->z <= other.z);
    }

    double x, y, z;
};

// typedef для разделения сущностей точки и вектора, хотя по сути это одно и то же.
typedef Vector3d Point;

static std::istream &operator>>(std::istream &input, Vector3d &vector) {
    input >> vector.x >> vector.y >> vector.z;
    return input;
}

static std::ostream &operator<<(std::ostream &output, Vector3d &vector) {
    output << vector.x << ' ' << vector.y << ' ' << vector.z;
    return output;
}

// Представляет луч, исходящий из точки origin по единичному вектору направления direction.
class Ray {
public:

    Ray(const Point &origin, const Vector3d &direction) : origin(origin), direction(direction.normalize()) { }

    const Point &getOrigin() const {
        return origin;
    }

    const Vector3d &getDirection() const {
        return direction;
    }

private:
    Point origin;
    Vector3d direction;
};

struct BoundingBox {

    BoundingBox() { }

    BoundingBox(const Point &minCorner, const Point &maxCorner) : minCorner(minCorner), maxCorner(maxCorner) { }

    void extend(const BoundingBox &otherBox) {
        minCorner = getMinPoint(minCorner, otherBox.minCorner);
        maxCorner = getMaxPoint(maxCorner, otherBox.maxCorner);
    }

    Point minCorner, maxCorner;

    BoundingBox(const std::vector<Point> &points) {
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

    double surfaceArea() const {
        double area = 0;
        area += fabs((maxCorner.x - minCorner.x) * (maxCorner.y - minCorner.y));
        area += fabs((maxCorner.y - minCorner.y) * (maxCorner.z - minCorner.z));
        area += fabs((maxCorner.z - minCorner.z) * (maxCorner.x - minCorner.x));
        return area * 2;
    }


private:
    Point getMinPoint(const Point &first, const Point &second) {
        return Point(std::min(first.x, second.x),
                     std::min(first.y, second.y),
                     std::min(first.z, second.z));
    }

    Point getMaxPoint(const Point &first, const Point &second) {
        return Point(std::max(first.x, second.x),
                     std::max(first.y, second.y),
                     std::max(first.z, second.z));
    }
};

#endif //RAYTRACER_GEOMETRY_H
