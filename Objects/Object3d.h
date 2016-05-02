#ifndef RAYTRACER_OBJECT3D_H
#define RAYTRACER_OBJECT3D_H

#include "../Geometry/Geometry.h"
#include "../Picture.h"
#include "../Geometry/Vector3d.h"
#include "../Geometry/Ray.h"
#include "../Geometry/BoundingBox.h"

struct Material {

    Material(const Color &color = CL_WHITE, double reflectance = 0,
             double refractiveIndex = 1, double transparency = 0,
             double lambert = 0.4, double phong = 0.6, double phongPower = 3) : //todo: подобрать коэфиценты
            color(color), reflectance(reflectance),
            refractiveIndex(refractiveIndex),
            transparency(transparency),
            lambert(lambert), phong(phong),
            phongPower(phongPower) { }

    const Color color;
    const double reflectance;
    const double refractiveIndex;
    const double transparency;
    const double lambert;
    const double phong, phongPower;
};

// Интерфейс представляет обобщенный объект сцены.
class Object3d {
public:

    Object3d(const Material &material) : material(material) { }

    virtual Vector3d getNormal(const Point &) const = 0;

    virtual bool intersectRay(const Ray &, Point &intersection) const = 0;

    virtual Color getColorAt(const Point &point) const = 0;

    virtual BoundingBox getBoundingBox() const = 0;

    virtual ~Object3d() {}

    const Material &getMaterial() const {
        return material;
    }

protected:
    Material material;
};


#endif //RAYTRACER_OBJECT3D_H
