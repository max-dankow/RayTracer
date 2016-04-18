#include "Sphere.h"

bool Sphere::intersectRay(const Ray &ray, Point &intersection) const {
    Vector3d centerToOrigin = ray.getOrigin() - center;
    // a = 0, т.к. нормируем вектор направления луча.
    double b = (Vector3d::dotProduct(ray.getDirection().normalize(), centerToOrigin));
    double c = centerToOrigin.lengthSquared() - radiusSquared;
    double d = b * b - c;
    if (d >= 0) {
        double sqrtOfD = sqrt(d);
        double t1 = -b + sqrtOfD;
        double t2 = -b - sqrtOfD;
        double t_min = std::min(t1, t2);
        double t_max = std::max(t1, t2);
        double t_closest = (t_min >= 0) ? t_min : t_max;
        if (t_closest > 0) {
            intersection = ray.getPointAt(t_closest);
            return true;
        } else {
            return false;
        }
    }
    return false;
}


BoundingBox Sphere::getBoundingBox() const {
    return BoundingBox(Point(center - Point(1, 1, 1) * radius), Point(center + Point(1, 1, 1) * radius));
}
