#ifndef RAYTRACER_TRIANGLE3D_H
#define RAYTRACER_TRIANGLE3D_H

#include "Object3d.h"

class Triangle3d : public Object3d {
public:

    Triangle3d(const Point &a, const Point &b, const Point &c, const Color &color);

    virtual Vector3d getNormal(Point point);

    virtual bool intersectRay(const Ray &ray, Point &intersection);

    virtual Color getColor(const Point &point);

private:
    // Вершины треугольника.
    const Point a, b, c;
    // Предпосчитанный вектор нормали.
    Vector3d normal;
    Color color;
};


#endif //RAYTRACER_TRIANGLE3D_H
