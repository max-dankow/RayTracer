#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include <exception>
#include "../Geometry/Vector3d.h"
#include "../Picture.h"
#include "Object3d.h"

class Sphere : public Object3d{
public:

    Sphere() {}

    Sphere(const Point &center, double radius,
           const Color &color, double reflectance) :
            center(center), radius(radius), radiusSquared(radius * radius),
            color(color), reflectance(reflectance) {
        assert(radius > 0 && !Geometry::areDoubleEqual(radius, 0));
    }

    const Point &getCenter() const {
        return center;
    }

    double getRadius() const {
        return radius;
    }

    double getRadiusSquared() const {
        return radiusSquared;
    }

    virtual double getReflectance() const {
        return reflectance;
    }

    virtual Vector3d getNormal(const Point &point) const {
        assert(Geometry::areDoubleEqual((point - center).lengthSquared(), radiusSquared));
        return Vector3d(point - center).normalize();
    }

    virtual Color getColor(const Point &point) const {
        return color;
    }

    virtual bool intersectRay(const Ray &ray, Point &intersection) const;

    virtual BoundingBox getBoundingBox() const;

private:
    Point center;
    double radius, radiusSquared;
    Color color;
    double reflectance;
};


#endif //RAYTRACER_SPHERE_H
