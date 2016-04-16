#ifndef RAYTRACER_OBJECT3D_H
#define RAYTRACER_OBJECT3D_H

#include "../Geometry/Geometry.h"
#include "../Picture.h"
#include "../Geometry/Vector3d.h"
#include "../Geometry/Ray.h"
#include "../Geometry/BoundingBox.h"

// Интерфейс представляет обобщенный объект сцены.
class Object3d {
public:
    virtual Vector3d getNormal(const Point &) const = 0;
    virtual bool intersectRay(const Ray &, Point &intersection) const = 0;
    virtual Color getColor(const Point &point) const = 0;
    virtual double getReflectance() const = 0;
    virtual BoundingBox getBoundingBox() const = 0;
    virtual bool isIntersectBox(const BoundingBox &box) const = 0;
    Object3d(){}// todo: разобраться с наследованием
    virtual ~Object3d() {};
};


#endif //RAYTRACER_OBJECT3D_H
