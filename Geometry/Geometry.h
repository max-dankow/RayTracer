#ifndef RAYTRACER_GEOMETRY_H
#define RAYTRACER_GEOMETRY_H

#include <cmath>
#include <limits>

typedef float Real;

enum Axis : char {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2
};

class Geometry {
public:
    static bool areRealNumbersEqual(const Real a, const Real b) {
        return (std::abs(a - b) < PRECISION);
    }

    static Axis nextAxis(Axis axis) {
        return Axis((int(axis) + 1) % 3);
    }

    static constexpr Real INFINITY_DOUBLE = std::numeric_limits<Real>::infinity();

    static constexpr Real PRECISION = 1e-6;
};

#endif //RAYTRACER_GEOMETRY_H
