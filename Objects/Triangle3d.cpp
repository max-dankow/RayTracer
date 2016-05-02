#include "Triangle3d.h"

Triangle3d::Triangle3d(const Point &a, const Point &b, const Point &c,
                       const Material &material) : Object3d(material), a(a), b(b), c(c) {
    ab = Vector3d(a, b);
    bc = Vector3d(b, c);
    ca = Vector3d(c, a);
    Vector3d ac = Vector3d(a, c);
    normal = Vector3d::crossProduct(ab, ac).normalize();
    D = Vector3d::dotProduct(normal, a);
}

Vector3d Triangle3d::getNormal(const Point &point) const {
    return normal;
}

bool Triangle3d::intersectRay(const Ray &ray, Point &intersection) const {
    double t = 0;
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

Color Triangle3d::getColorAt(const Point &point) const {
    return material.color;
}

BoundingBox Triangle3d::getBoundingBox() const {
    return BoundingBox({a, b, c});
}