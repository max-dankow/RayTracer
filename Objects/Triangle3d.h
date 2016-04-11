#ifndef RAYTRACER_TRIANGLE3D_H
#define RAYTRACER_TRIANGLE3D_H

#include "Object3d.h"

// Представляет треугольник в пространстве, задаваемый тремя вершинами.
// Нормаль определяется по правилу правой руки.
// Имеет только лицевую сторону - ту, которая по направлению нормали.
// Другая сторона считается невидимой (внутренней).
class Triangle3d : public Object3d {
public:

    Triangle3d(const Point &a, const Point &b, const Point &c, const Color &color = CL_WHITE);

    virtual Vector3d getNormal(const Point &point) const;

    virtual bool intersectRay(const Ray &ray, Point &intersection) const;

    virtual Color getColor(const Point &point) const;

    virtual BoundingBox getBoundingBox() const;


private:
    // Вершины треугольника.
    Point a, b, c;
    Color color;
    // Предпосчитанный вектор нормали.
    Vector3d normal;
    // Для некоторого ускорения добавим предпосчитанные величины.
    Vector3d ab, bc, ca;  // Предпосчитанные векторы сторон
    double D;  // Коэфицент при свободном члене уравнения плоскости треугольника
};

#endif //RAYTRACER_TRIANGLE3D_H
