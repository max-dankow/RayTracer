#include "Triangle3d.h"

Triangle3d::Triangle3d(const Point &a, const Point &b, const Point &c, const Color &color) :
        a(a), b(b), c(c), color(color) {
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
    double dotNormalAndDir = Vector3d::dotProduct(normal, ray.getDirection());

    // Проверяем параллельны ли луч и плоскость треугольника.
    if (areDoubleEqual(dotNormalAndDir, 0)) {
        return false;
    }

    // Решаем систему уравнений и находим t.
    double t = - (Vector3d::dotProduct(normal, ray.getOrigin()) - D) / dotNormalAndDir;

    // Проверяем лежит ли точка пересечения с обратной стороны луча.
    if (t < 0) {
        return false;
    }

    Point p = ray.getOrigin() + ray.getDirection() * t;  // Точка пересечения луча с плоскостью треугольника
    // Проверяем лежит ли p внутри треугольника.
    Vector3d ap(a, p), bp(b, p), cp(c, p);
    if (Vector3d::dotProduct(normal, Vector3d::crossProduct(ab, ap)) >= 0
        && Vector3d::dotProduct(normal, Vector3d::crossProduct(bc, bp)) >= 0
        && Vector3d::dotProduct(normal, Vector3d::crossProduct(ca, cp)) >= 0) {
        intersection = p;
        return true;
    }
    return false;
}

Color Triangle3d::getColor(const Point &point) const {
    return color;
}

BoundingBox Triangle3d::getBoundingBox() const {
    return BoundingBox({a, b, c});
}

bool Triangle3d::isIntersectBox(const BoundingBox &box) const {
    return getBoundingBox().intersectBox(box);
    // todo: ЭТО ДОСТАНОЧНОЕ НО НЕ НЕОБХОДИМОЕ УСЛОВИЕ!!!
    // Если хотя бы одна точка лежит между minCorner и maxCorner, то треугольник пересекает box.
//    return  (box.containsPoint(a) || box.containsPoint(b) || box.containsPoint(c));
}