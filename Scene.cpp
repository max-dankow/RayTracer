#include "Scene.h"
#include <limits>

Picture Scene::render() {
    Vector3d colVector = Vector3d(screenBottomRight.x - screenTopLeft.x,
                                  0, screenBottomRight.z - screenTopLeft.z) * (1. / pixelNumberWidth);
    Vector3d rowVector = Vector3d(0, screenBottomRight.y - screenTopLeft.y, 0) * (1. / pixelNumberHeight);
    Picture picture(pixelNumberWidth, pixelNumberHeight);
    Point intersection;
    Color finalColor;
//    if (!castRay(Ray(viewPoint, Point(0, 0, 1)), 1, intersection, finalColor)) {
//        finalColor = backgroundColor;
//    }
    std::cout << "Start rendering scene...\n";
    for (size_t col = 0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0; row < pixelNumberHeight; ++row) {
            // Смещаем 0.5 чтобы попасть в серединку пикселя.
            Point pixel(colVector * (0.5 + col) + rowVector * (0.5 + row) + screenTopLeft);
            if (!castRay(Ray(viewPoint, pixel - viewPoint), 1, intersection, finalColor)) {
                finalColor = backgroundColor;
            }
            picture.setAt(col, row, finalColor);
        }
        if (col == pixelNumberWidth - 1 || col % 10 == 0) {
            std::cout << "Rendering " << 100 * col / pixelNumberWidth << "%\n";
        }
    }

    return picture;
}

bool Scene::castRay(const Ray &ray, int restDepth, Point &intersection, Color &finalColor) {
    assert(restDepth >= 0); // todo: assert замедляет.
    double minDistance = 0;
    std::list<unique_ptr<Object3d>>::const_iterator intersector;  // Итератор соотвествующий объекте пересечения.
//    double distanceToScreen = ray.getDirection().length();
    bool haveAny = false;
//    for (const unique_ptr<Object3d> &object : objects) {
    for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
        Point thisIntersection;
        if ((*iter)->intersectRay(ray, thisIntersection)) {
            double distance = (thisIntersection - ray.getOrigin()).length();
            if ((!haveAny || distance < minDistance) && !areDoubleEqual(distance, 0)) {
                haveAny = true;
                minDistance = distance;
                intersection = thisIntersection;
                finalColor = (*iter)->getColor(thisIntersection);
                intersector = iter;
            }
        }
    }
    if (!haveAny) {
        return false;
    }
    if (restDepth == 0) {
        return true;
    }
    Color hsv = rgb2hsv(finalColor);
    double totalBrightness = 0;
    for (const unique_ptr<LightSource> &light : lights) {
        Ray lightRay(intersection, light->getPoint() - intersection);
        double dotProduct = Vector3d::dotProduct((*intersector)->getNormal(intersection), lightRay.getDirection().normalize());
        if (dotProduct <= 0 || areDoubleEqual(lightRay.getDirection().length(), 0)) {  // Источник находится за поверхностью
            continue;
        }
        Point obstacleHitPoint;
        Color obstacleColor;
        // Если луч не прервался перпятствием.
        if (!castRay(lightRay, 0, obstacleHitPoint, obstacleColor)) {
            double brightness = dotProduct / (lightRay.getDirection().length() * lightRay.getDirection().length());
            totalBrightness += brightness * light->getBrightess();
        }
    }
    hsv.v *= totalBrightness;
    if (hsv.v > 1) {
        hsv.v = 1;
    }
    finalColor = hsv2rgb(hsv);
    return true;
}

void Scene::emplaceObject(Object3d *object) {
    objects.emplace_back(object);
}


