#ifndef RAYTRACER_LIGHTSOURCE_H
#define RAYTRACER_LIGHTSOURCE_H


#include "Picture.h"
#include "Geometry.h"

class LightSource {
public:

    LightSource(const Point &point, const Color &color = CL_WHITE) : point(point), color(color) { }

    const Point &getPoint() const {
        return point;
    }

    void setPoint(const Point &point) {
        LightSource::point = point;
    }

    const Color &getColor() const {
        return color;
    }

    void setColor(const Color &color) {
        LightSource::color = color;
    }

private:
    Point point;
    Color color;
};

#endif //RAYTRACER_LIGHTSOURCE_H
