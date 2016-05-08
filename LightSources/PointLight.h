#ifndef RAYTRACER_POINTLIGHT_H
#define RAYTRACER_POINTLIGHT_H

#include "LightSource.h"
#include "../Geometry/Vector3d.h"
#include "../Geometry/Ray.h"

class PointLight : public LightSource {
public:
    PointLight(const Point &point, double brightness, const Color &color = CL_WHITE) :
            LightSource(point, brightness, color) { }

    virtual Photon emitPhoton() { // todo: может можно избавиться от сложных рассчетов. или они не сложные?
        std::uniform_real_distribution<double> angleDistribution(0, M_PI * 2);
        std::uniform_real_distribution<double> thetaDistribution(0, M_PI);
        double theta = thetaDistribution(randomEngine);
        double phi = angleDistribution(randomEngine);
        Vector3d direction(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        return Photon(getPoint(), direction, getColor() * (getBrightness() / 10000.));
    }

private:
    std::default_random_engine randomEngine;

};


#endif //RAYTRACER_POINTLIGHT_H
