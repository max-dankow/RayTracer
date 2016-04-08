#ifndef RAYTRACER_LIGHTSOURCE_H
#define RAYTRACER_LIGHTSOURCE_H


#include "Picture.h"
#include "Geometry.h"

class LightSource {
public:

    LightSource(const Point &point, double brightness = 1, const Color &color = CL_WHITE) :
            point(point), brightess(brightness), color(color) { }

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

    double getBrightess() const {
        return brightess;
    }

    void setBrightess(double brightess) {
        LightSource::brightess = brightess;
    }

private:
    Point point;
    double brightess;
    Color color;
};

#endif //RAYTRACER_LIGHTSOURCE_H
