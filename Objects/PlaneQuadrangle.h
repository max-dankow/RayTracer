#ifndef RAYTRACER_PLANEQUADRANGLE_H
#define RAYTRACER_PLANEQUADRANGLE_H


#include <array>
#include "Object3d.h"
// todo: все неправильно
class PlaneQuadrangle : public Object3d {
public:
    PlaneQuadrangle(const std::array<Point, 4> &vertices, const Material *material) :
            Object3d(material), vertices(vertices) {
        std::vector<Vector3d> crossProducts(4);
        for (size_t i = 0; i < 4; ++i) {
            Vector3d currentEdge(vertices[i], vertices[(i + 1) % 4]);
            Vector3d nextEdge(vertices[(i + 1) % 4], vertices[(i + 2) % 4]);
            crossProducts[(i + 1) % 4] = Vector3d::crossProduct(currentEdge, nextEdge). normalize();
        }
        // todo: больше ограничний и исключений.
        // Ищем настоящую нормаль.
        for (size_t i = 0; i < crossProducts.size(); ++i) {
            if (!Vector3d::isNullVector(Vector3d::crossProduct(crossProducts[i], crossProducts[(i + 1) % 4]))) {
                //todo : сделать метод проверки на коллинеарность и сонаправленнойсть
                throw std::invalid_argument("All points should be in the same plane");
            }
            if (crossProducts[i] == crossProducts[(i + 1) % 4]) {
                normal = crossProducts[i];
            }
        }
        // Ищем невыпуклую вершину, если она есть.
        badVertexIndex = 0;
        for (size_t i = 0; i < crossProducts.size(); ++i) {
            if (crossProducts[i] != normal) {
                badVertexIndex = i;
                break;
            }
        }
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
        if (p.belongsToTriangle(vertices[badVertexIndex],
                                vertices[(badVertexIndex + 1) % 4],
                                vertices[(badVertexIndex + 2) % 4],
                                normal)
            || p.belongsToTriangle(vertices[badVertexIndex],
                                   vertices[(badVertexIndex + 2) % 4],
                                   vertices[(badVertexIndex + 3) % 4],
                                   normal)) {
            intersection = p;
            return true;
        }
        return false;
    }

    virtual Color getColorAt(const Point &point) const {
        return material->color;
    }

    virtual BoundingBox getBoundingBox() const {
        return BoundingBox(std::vector<Point>(std::begin(vertices), std::end(vertices)));
    }

private:
    std::array<Point, 4> vertices;
    size_t badVertexIndex;
    Vector3d normal;
};



#endif //RAYTRACER_PLANEQUADRANGLE_H
