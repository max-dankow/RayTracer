#ifndef RAYTRACER_OBJECT3D_H
#define RAYTRACER_OBJECT3D_H

#include "../Geometry.h"

class Object3d {
public:
    virtual Vector3d getNormal(Point)=0;
    virtual bool intersectRay(const Ray &, Point &intersection) =0;
};


#endif //RAYTRACER_OBJECT3D_H
