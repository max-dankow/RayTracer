#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <cmath>
#include <list>
#include <memory>
#include "Objects/Object3d.h"
#include "Geometry.h"
#include "Painter/Painter.h"
#include "LightSource.h"

using std::unique_ptr;

class Scene {
public:

    Scene(const Point &viewPoint,
          const Point &screenTopLeft,
          const Point &screenBottomRight,
          size_t pixelNumberWidth,
          size_t pixelNumberHeight,
          std::vector<unique_ptr<Object3d>> &&objects,
          std::vector<unique_ptr<LightSource>> &&lights) :
            viewPoint(viewPoint),
            screenTopLeft(screenTopLeft),
            screenBottomRight(screenBottomRight),
            pixelNumberWidth(pixelNumberWidth),
            pixelNumberHeight(pixelNumberHeight),
            objects(std::move(objects)),
            lights(std::move(lights)){ }

    Picture render();

    void emplaceObject(Object3d *object);

private:
    bool castRay(const Ray &ray, int restDepth, Point &intersection, Color &finalColor);

    // Точка камеры.
    Point viewPoint;

    // Верхний левый и правый нижний угол экрана.
    // Считаем что стороны экрана параллельны осям.
    // todo: нормальный формат камеры
    Point screenTopLeft, screenBottomRight;

    size_t pixelNumberWidth, pixelNumberHeight;
    Color backgroundColor = CL_BLACK;
    double backgroundIllumination = 0.1;

    // Указатели на все объекты сцены, хранимые в куче.
    std::vector<unique_ptr<Object3d> > objects;

    // Указатели на все источники освещения, так же хранимые в куче.
    std::vector<unique_ptr<LightSource> > lights;
};


#endif //RAYTRACER_SCENE_H
