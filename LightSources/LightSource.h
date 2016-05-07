#ifndef RAYTRACER_LIGHTSOURCE_H
#define RAYTRACER_LIGHTSOURCE_H

#include <random>
#include <cmath>
#include "../Picture.h"
#include "../Geometry/Geometry.h"

class Photon {
public:

    Photon() {}

    Photon(const Point &position, const Vector3d &direction, const Color &color) :
            ray(position, direction), color(color) { }

    Photon(const Ray &ray, const Color &color) : ray(ray), color(color) { }

    const Ray &getRay() const {
        return ray;
    }

    const Vector3d &getDirection() const {
        return ray.getDirection();
    }

    const Color &getColor() const {
        return color;
    }

private:
    Ray ray;
    Color color; // todo : интерпретировать цвет лучей как энергию
};

// Представляет источник освещения, расположенный в точке point и яркостью brightness.
class LightSource {
public:


    LightSource(const Point &point, double brightness, const Color &color = CL_WHITE) :
            point(point), brightness(brightness), color(color) { }

    const Color &getColor() const {
        return color;
    }

    double getBrightness() const {
        return brightness;
    }

    const Point &getPoint() const { // todo: точечность присуща только точечным источникам разобраться
        return point;
    }

    virtual Photon emitPhoton() = 0;

    virtual ~LightSource() {}

private:
    Point point;
    double brightness;
    Color color;
};

#endif //RAYTRACER_LIGHTSOURCE_H
