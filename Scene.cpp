#include "Scene.h"
#include <limits>

Picture Scene::render() {
    Vector3d colVector = Vector3d(screenBottomRight.x - screenTopLeft.x,
                                  0, screenBottomRight.z - screenTopLeft.z) * (1. / pixelNumberWidth);
    Vector3d rowVector = Vector3d(0, screenBottomRight.y - screenTopLeft.y, 0) * (1. / pixelNumberHeight);
    Picture picture(pixelNumberWidth, pixelNumberHeight);
//    castRay(Ray(viewPoint, Point(0, 0, 1)), 0, <#initializer#>, <#initializer#>);
    Point intersection;
    Color finalColor;
    if (!castRay(Ray(viewPoint, Point(0, 0, 1)), 1, intersection, finalColor)) {
        finalColor = backgroundColor;
    }
    //picture.setAt(col, row, finalColor);
    for (size_t col = 0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0; row < pixelNumberHeight; ++row) {
            // Смещаем 0.5 чтобы попасть в серединку пикселя.
            Point pixel(colVector * (0.5 + col) + rowVector * (0.5 + row) + screenTopLeft);
//            Point intersection;
//            Color finalColor;
            if (!castRay(Ray(viewPoint, pixel - viewPoint), 1, intersection, finalColor)) {
                finalColor = backgroundColor;
            }
            picture.setAt(col, row, finalColor);
        }
    }

    return picture;
}

bool Scene::castRay(const Ray &ray, int restDepth, Point &intersection, Color &finalColor) {
    assert(restDepth >= 0); // todo: assert замедляет.
    double minDistance = 0;
//    double distanceToScreen = ray.getDirection().length();
    bool haveAny = false;
    for (const unique_ptr<Object3d> &object : objects) {
        Point thisIntersection;
        if (object->intersectRay(ray, thisIntersection)) {
            double distance = (thisIntersection - ray.getOrigin()).length();
            if ((!haveAny || distance < minDistance) && !areDoubleEqual(distance, 0)) {
                haveAny = true;
                minDistance = distance;
                intersection = thisIntersection;
                finalColor = object->getColor(thisIntersection);
            }
        }
    }
    if (!haveAny) {
        return false;
    }
    // Испускаем луч во все источники освещения.
    if (restDepth > 0) {
        bool isShadowed = false;
        for (const unique_ptr<LightSource> &light : lights) {
            Ray lightRay(intersection, light->getPoint() - intersection);
            if (Vector3d::dotProduct(ray.getDirection(), lightRay.getDirection()) >= 0) {
                continue;
            }
            Point obstacleHitPoint;
            Color obstacleColor;
            if (castRay(lightRay, 0, obstacleHitPoint, obstacleColor)) { // todo: выяснить где какая глубина нужна.
                isShadowed = true;
            }
        }
        if (isShadowed) {
            finalColor = CL_BLACK;
        }
    }
    return true;
}
