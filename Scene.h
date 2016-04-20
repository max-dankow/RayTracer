#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <cmath>
#include <list>
#include <memory>
#include "Objects/Object3d.h"
#include "Geometry/Geometry.h"
#include "Painter/Painter.h"
#include "LightSource.h"
#include "KdTree.h"

using std::unique_ptr;

// todo: устранить безобразие с памятью!
class Scene {
public:

    Scene(const Point &viewPoint,
          const Point &screenTopLeft,
          const Point &screenBottomRight,
          size_t pixelNumberWidth,
          size_t pixelNumberHeight,
          const std::vector<Object3d*> &objects,
          std::vector<LightSource*> &&lights) :
            viewPoint(viewPoint),
            screenTopLeft(screenTopLeft),
            screenBottomRight(screenBottomRight),
            pixelNumberWidth(pixelNumberWidth),
            pixelNumberHeight(pixelNumberHeight),
            objects(objects),
            lights(std::move(lights)){ }

    Picture render();

//    void emplaceObject(Object3d *object);

private:
    bool castRay(const Ray &ray, int restDepth, Point &hitPoint, Color &finalColor);

    Object3d *checkIntersection(const Ray &ray,
                                const std::vector<Object3d *> objectList,
                                Point &intersection, Color &color);

    Object3d * findObstacle(const Ray &ray, Point &hitPoint, Color &obstacleColor);
    Color computeDiffuseColor(Object3d *object, const Point &point, const Ray &viewRay);
    Color computeReflectionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth);
    Color computeRefractionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth);
    static Vector3d refractRay(const Vector3d &direction, const Vector3d &normal, double q);
    static Vector3d reflectRay(const Vector3d &direction, const Vector3d &normal);

    // Точка камеры.
    Point viewPoint;

    // Верхний левый и правый нижний угол экрана.
    // Считаем что стороны экрана параллельны осям.
    // todo: нормальный формат камеры
    Point screenTopLeft, screenBottomRight;

    size_t pixelNumberWidth, pixelNumberHeight;
    Color backgroundColor = CL_BLACK;
    double backgroundIllumination = 0.025;

    // Kd дерево указателей на все объекты сцены, хранимые в куче.
    KdTree objects;
    // Указатели на все источники освещения, так же хранимые в куче.
    std::vector<LightSource*> lights;
};


#endif //RAYTRACER_SCENE_H
