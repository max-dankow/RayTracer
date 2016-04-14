#ifndef RAYTRACER_GEOMETRY_H
#define RAYTRACER_GEOMETRY_H

#include <cmath>
#include <vector>
#include <assert.h>
#include <limits>
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


static Axis nextAxis(Axis axis) {
    return Axis ((int(axis) + 1) % 3);
}

class Vector3d {
public:
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
        return ((this->x <= other.x || areDoubleEqual(this->x, other.x))
                && (this->y <= other.y || areDoubleEqual(this->y, other.y))
                && (this->z <= other.z || areDoubleEqual(this->z, other.z)));
    }

    double x, y, z;
};

// typedef для разделения сущностей точки и вектора, хотя по сути это одно и то же.
typedef Vector3d Point;

static std::istream &operator>>(std::istream &input, Vector3d &vector) {
    input >> vector.x >> vector.y >> vector.z;
    return input;
}

static std::ostream &operator<<(std::ostream &output, const Vector3d &vector) {
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

    Point getPointAt(double t) const {
        if (t < 0) {
            return origin;
        } else {
            return origin + direction * t;
        }
    }

    // todo: в тругольнике и тут используется обдо и тоже пересечение с плоскостью.
    bool intersectPlane(const Vector3d &a, const Vector3d &b, const Point& point, double &intersection) const {
        Vector3d normal = Vector3d::crossProduct(a, b);
        double D = Vector3d::dotProduct(normal, point);

        double dotNormalAndDir = Vector3d::dotProduct(normal, direction);
        if (areDoubleEqual(dotNormalAndDir, 0)) {
            return false;
        }

        // Решаем систему уравнений и находим t.
        double t = - (Vector3d::dotProduct(normal, origin) - D) / dotNormalAndDir;
        if (t < 0) {
            return false;
        } else {
            intersection = t;
            return true;
        }
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

    bool containsPoint(const Point &point)const {
        return point.totallyLessEqualThan(maxCorner) && minCorner.totallyLessEqualThan(point);
    }

    bool intersectBox(const BoundingBox &other) const {
        return this->minCorner.totallyLessEqualThan(other.maxCorner)
               && other.minCorner.totallyLessEqualThan(this->maxCorner);
    }

    double intersectRay(const Ray &ray) const {
        if (containsPoint(ray.getOrigin())) {
            return 0;
        }
        double t, t_min = 0;
        bool haveAny = false;
        auto minCornerLocal = minCorner;
        auto maxCornerLocal = maxCorner;
//        Vector3d ox(maxCorner.x - minCorner.x, 0, 0);
//        Vector3d oy(0, maxCorner.y - minCorner.y, 0);
//        Vector3d oz(0, 0, maxCorner.z - minCorner.z);

        // Переберем все плоскости и найдем для всех пересечение с лучом.
        for (int axisIter = 0; axisIter < 3; ++axisIter) {
            Axis axis1 = static_cast<Axis> (axisIter);
            Axis axis2 = nextAxis(axis1);
            Vector3d v1(0, 0, 0);
            v1[axis1] = maxCornerLocal[axis1] - minCornerLocal[axis1];
            Vector3d v2(0, 0, 0);
            v2[axis2] = maxCornerLocal[axis2] - minCornerLocal[axis2];
            if (ray.intersectPlane(v1, v2, minCornerLocal, t)) {
                // Если точка пересечения с плоскостью лежит в боксе, то обновим минимум.
                if (containsPoint(ray.getPointAt(t))) {
                    if (!haveAny || t < t_min) {
                        t_min = t;
                        haveAny = true;
                    }
                }
            }
        }
        for (int axisIter = 0; axisIter < 3; ++axisIter) {
            Axis axis1 = static_cast<Axis> (axisIter);
            Axis axis2 = nextAxis(axis1);
            Vector3d v1(0, 0, 0);
            v1[axis1] = minCornerLocal[axis1] - maxCornerLocal[axis1];
            Vector3d v2(0, 0, 0);
            v2[axis2] = minCornerLocal[axis2] - maxCornerLocal[axis2];
            if (ray.intersectPlane(v1, v2, maxCornerLocal, t)) {
                // Если точка пересечения с плоскостью лежит в боксе, то обновим минимум.
                if (containsPoint(ray.getPointAt(t))) {
                    if (!haveAny || t < t_min) {
                        t_min = t;
                        haveAny = true;
                    }
                }
            }
        }
        if (haveAny) {
            return t_min;
        } else {
            return std::numeric_limits<double>::infinity(); // todo: do it better
        }
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
