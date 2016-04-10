#ifndef RAYTRACER_LIGHTSOURCE_H
#define RAYTRACER_LIGHTSOURCE_H

#include "Picture.h"
#include "Geometry.h"

// Представляет источник освещения, расположенный в точке point и яркостью brightness.
class LightSource {
public:

    LightSource(const Point &point, double brightness = 1, const Color &color = CL_WHITE) :
            point(point), brightness(brightness), color(color) { }

    const Point &getPoint() const {
        return point;
    }

    const Color &getColor() const {
        return color;
    }

    double getBrightness() const {
        return brightness;
    }

private:
    Point point;
    double brightness;
    Color color;
};

#endif //RAYTRACER_LIGHTSOURCE_H
