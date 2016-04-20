#ifndef RAYTRACER_TRIANGLE3D_H
#define RAYTRACER_TRIANGLE3D_H

#include "Object3d.h"

// Представляет треугольник в пространстве, задаваемый тремя вершинами.
// Нормаль определяется по правилу правой руки.
// Имеет только лицевую сторону - ту, которая по направлению нормали.
// Другая сторона считается невидимой (внутренней).
class Triangle3d : public Object3d {
public:

    Triangle3d(const Point &a, const Point &b, const Point &c, const Color &color, double reflectance,
                   double transparency);

    virtual Vector3d getNormal(const Point &point) const;

    virtual bool intersectRay(const Ray &ray, Point &intersection) const;

    virtual Color getColor(const Point &point) const;

    virtual double getReflectance() const {
        return reflectance;
    }

    virtual BoundingBox getBoundingBox() const;

    virtual ~Triangle3d() { }

private:
    // Вершины треугольника.
    Point a, b, c;
    Color color;
    // Коэффициент отражения.
    double reflectance;
    // Предпосчитанный вектор нормали.
    Vector3d normal;
    // Для некоторого ускорения добавим предпосчитанные величины.
    Vector3d ab, bc, ca;  // Предпосчитанные векторы сторон
    double D;  // Коэфицент при свободном члене уравнения плоскости треугольника
};

#endif //RAYTRACER_TRIANGLE3D_H
