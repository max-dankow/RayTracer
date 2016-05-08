#ifndef RAYTRACER_GEOMETRICSHAPE_H
#define RAYTRACER_GEOMETRICSHAPE_H


#include "BoundingBox.h"

// Интерфейс, представляющий пространственную геометрическую фигуру.
class GeometricShape {
public:
    virtual Vector3d getNormal(const Point &) const = 0;

    virtual bool intersectRay(const Ray &, Point &intersection) const = 0;

    virtual BoundingBox getBoundingBox() const = 0;

    virtual ~GeometricShape() { }
};


#endif //RAYTRACER_GEOMETRICSHAPE_H
