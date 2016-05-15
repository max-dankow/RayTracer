#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include <exception>
#include "../Geometry/Vector3d.h"
#include "../Picture.h"
#include "Object3d.h"

class Sphere : public Object3d{
public:

    Sphere(const Material *material) : Object3d(material) {}

    Sphere(const Point &center, Real radius, const Material *material) : Object3d(material),
            center(center), radius(radius), radiusSquared(radius * radius) {
        assert(radius > 0 && !Geometry::areRealNumbersEqual(radius, 0));
    }

    const Point &getCenter() const {
        return center;
    }

    Real getRadius() const {
        return radius;
    }

    Real getRadiusSquared() const {
        return radiusSquared;
    }

    virtual Vector3d getNormal(const Point &point) const {
        assert(Geometry::areRealNumbersEqual((point - center).lengthSquared(), radiusSquared));
        return Vector3d(point - center).normalize();
    }

    virtual Color getColorAt(const Point &point) const {
        return material->color;
    }

    virtual bool intersectRay(const Ray &ray, Point &intersection) const;

    virtual BoundingBox getBoundingBox() const;

private:
    Point center;
    Real radius, radiusSquared;
};


#endif //RAYTRACER_SPHERE_H
