#ifndef RAYTRACER_TRIANGLE3D_H
#define RAYTRACER_TRIANGLE3D_H

#include "Object3d.h"

// Представляет треугольник в пространстве, задаваемый тремя вершинами.
// Нормаль определяется по правилу правой руки.
// Имеет только лицевую сторону - ту, которая по направлению нормали.
// Другая сторона считается невидимой (внутренней).
class Triangle3d : public Object3d {
public:

    Triangle3d(const Point &a, const Point &b, const Point &c, const Material *material);

    virtual Vector3d getNormal(const Point &point) const {
        return normal;
    }

    virtual bool intersectRay(const Ray &ray, Point &intersection) const;

    virtual Color getColorAt(const Point &point) const {
        return material->color;
    }

    virtual BoundingBox getBoundingBox() const {
        return BoundingBox({a, b, c});
    }

    virtual ~Triangle3d() { }

private:
    // Вершины треугольника.
    Point a, b, c;
    // Предпосчитанный вектор нормали.
    Vector3d normal;
};

Triangle3d::Triangle3d(const Point &a, const Point &b, const Point &c,
                       const Material *material) : Object3d(material), a(a), b(b), c(c) {
    Vector3d ab(a, b), ac(a, c);
    normal = Vector3d::crossProduct(ab, ac).normalize();
}

bool Triangle3d::intersectRay(const Ray &ray, Point &intersection) const {
    Real t = 0;
    Real D = Vector3d::dotProduct(normal, a);
    if (!ray.intersectPlane(normal, D, a, t)) {
        return false;
    }
    Point p = ray.getPointAt(t);  // Точка пересечения луча с плоскостью треугольника
    // Проверяем лежит ли p внутри треугольника.
    if (p.belongsToTriangle(a, b, c, normal)) {
        intersection = p;
        return true;
    }
    return false;
}

#endif //RAYTRACER_TRIANGLE3D_H
