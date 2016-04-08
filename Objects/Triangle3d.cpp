#include "Triangle3d.h"

Vector3d Triangle3d::getNormal(Point point) {
    return normal;
}

Triangle3d::Triangle3d(const Point &a, const Point &b, const Point &c, const Color &color) :
        a(a), b(b), c(c), color(color) {
    Vector3d ab(a, b);
    Vector3d ac(a, c);
    normal = Vector3d::crossProduct(ab, ac).normalize();
}

bool Triangle3d::intersectRay(const Ray &ray, Point &intersection) {
    // Составляем уравнение плоскости треугольника.
    double D = Vector3d::dotProduct(normal, a);
    double dotNormalAndDir = Vector3d::dotProduct(normal, ray.getDirection());

    // Проверяем параллельны ли луч и плоскость треугольника.
    if (areDoubleEqual(dotNormalAndDir, 0)) {
        return false;
    }

    // Решаем систему уравнений и находим t.
    double t = - (Vector3d::dotProduct(normal, ray.getOrigin()) - D)
               / dotNormalAndDir;

    // Проверяем лежит ли точка пересечения с обратной стороны луча.
    if (t < 0) {
        return false;
    }

    Point p = ray.getOrigin() + ray.getDirection() * t;  // Точка пересечения луча с плоскостью треугольника
    // Проверяем лежит ли p внутри треугольника.
    Vector3d ab(a, b), bc(b, c), ca(c, a);
    Vector3d ap(a, p), bp(b, p), cp(c, p);
    if (Vector3d::dotProduct(normal, Vector3d::crossProduct(ab, ap)) >= 0
        && Vector3d::dotProduct(normal, Vector3d::crossProduct(bc, bp)) >= 0
        && Vector3d::dotProduct(normal, Vector3d::crossProduct(ca, cp)) >= 0) {
        intersection = p;
        return true;
    }
    return false;
}

Color Triangle3d::getColor(const Point &point) {
    return color;
}