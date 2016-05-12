#ifndef RAYTRACER_PARALLELOGRAM_H
#define RAYTRACER_PARALLELOGRAM_H

#include <array>
#include "Object3d.h"

class Parallelogram : public Object3d {
public:

    Parallelogram(const std::array<Point, 3> inputPoints,
                  const Material *material) : Object3d(material) {
        for (size_t i = 0; i < 3; ++i) { //todo: krasivee
            vertices[i] = inputPoints[i];
        }
        // Рассчитываем четвертую точку.
        Vector3d ab(vertices[0], vertices[1]);
        Vector3d bc(vertices[1], vertices[2]);
        vertices[3] = vertices[0] + bc;
        normal = Vector3d::crossProduct(ab, bc).normalize();
    }

    virtual Vector3d getNormal(const Point &point = Point()) const {
        return normal;
    }

    virtual bool intersectRay(const Ray &ray, Point &intersection) const {
        double t = 0;
        Vector3d ab(vertices[0], vertices[1]), ac(vertices[0], vertices[2]);
        if (!ray.intersectPlane(ab, ac, vertices[0], t)) {
            return false;
        }
        Point p = ray.getPointAt(t);
        auto normal = getNormal();
        for (size_t i = 0; i < 4; ++i) {
            auto edge = Vector3d(vertices[i], vertices[(i + 1) % 4]);
            auto vertexToPoint = Vector3d(vertices[i], p);
            if (Vector3d::dotProduct(normal, Vector3d::crossProduct(edge, vertexToPoint)) < 0) {
                return false;
            }
        }
        intersection = p;
        return true;
    }

    virtual Color getColorAt(const Point &point = Point()) const {
        return material->color;
    }

    virtual BoundingBox getBoundingBox() const {
        return BoundingBox(std::vector<Point>(std::begin(vertices), std::end(vertices)));
//        return BoundingBox({vertices[0], vertices[1], vertices[2], vertices[3]});
        // todo : const iterator
    }

private:
    std::array<Point, 4> vertices;
    Vector3d normal;
};


#endif //RAYTRACER_PARALLELOGRAM_H
