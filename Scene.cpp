#include "Scene.h"

Picture Scene::render() {
    Vector3d colVector = Vector3d(screenBottomRight.x - screenTopLeft.x,
                                  0, screenBottomRight.z - screenTopLeft.z) * (1. / pixelNumberWidth);
    Vector3d rowVector = Vector3d(0, screenBottomRight.y - screenTopLeft.y,
                                  screenBottomRight.z - screenTopLeft.z) * (1. / pixelNumberHeight);
    Picture picture(pixelNumberWidth, pixelNumberHeight);
    castRay( Ray(viewPoint,Point(0,0,1)));

    for (size_t col = 0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0; row < pixelNumberHeight; ++row) {
            // Смещаем 0.5 чтобы попасть в серединку пикселя.
            Point pixel(colVector * (0.5 + col) + rowVector * (0.5 + row) + screenTopLeft);
            Color color = castRay(Ray(viewPoint, pixel - viewPoint));
            picture.setAt(col, row, color);
        }
    }

    return picture;
}

Color Scene::castRay(const Ray &ray) {
    Color closestColor = backgroundColor;
    double minDistance = 0;
    bool forTheFirstTime = true;
    for (const unique_ptr<Object3d> &object : objects) {
        Point intersection;
        if (object->intersectRay(ray, intersection)) {
            double distance = (intersection - ray.getOrigin()).length();
            if (forTheFirstTime || distance < minDistance) {
                forTheFirstTime = false;
                minDistance = distance;
                closestColor = object->getColor(intersection);
            }
        }
    }
    return closestColor;
}



