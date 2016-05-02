#ifndef RAYTRACER_GEOMETRY_H
#define RAYTRACER_GEOMETRY_H

#include <cmath>
#include <limits>

enum Axis : char {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2
};

class Geometry {
public:
    static bool areDoubleEqual(const double a, const double b) {
        return (fabs(a - b) < PRECISION);
    }

    static Axis nextAxis(Axis axis) {
        return Axis((int(axis) + 1) % 3);
    }

    static constexpr double INFINITY_DOUBLE = std::numeric_limits<double>::infinity();

    static constexpr double PRECISION = 1e-6;
};

#endif //RAYTRACER_GEOMETRY_H
