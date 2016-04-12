#include <chrono>
#include "Scene.h"


Picture Scene::render() {
    Vector3d colVector = Vector3d(screenBottomRight.x - screenTopLeft.x,
                                  0, screenBottomRight.z - screenTopLeft.z) / pixelNumberWidth;
    Vector3d rowVector = Vector3d(0, screenBottomRight.y - screenTopLeft.y, 0) / pixelNumberHeight;
    Picture picture(pixelNumberWidth, pixelNumberHeight);
    auto startTime = std::chrono::steady_clock::now();

    std::cout << "Start rendering scene...\n";
    stats.intersectionAtemptCount = 0;
    stats.rayNumber = 0;
    for (size_t col = 0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0; row < pixelNumberHeight; ++row) {
            // Смещаем 0.5 чтобы попасть в серединку пикселя.
            Point pixel(colVector * (0.5 + col) + rowVector * (0.5 + row) + screenTopLeft);
            Point intersection;
            Color finalColor;
            if (!castRay(Ray(viewPoint, pixel - viewPoint), 1, intersection, finalColor)) {
                finalColor = backgroundColor;
            }
            picture.setAt(col, row, finalColor);
        }
        // Отображение прогресса.
        if (col == pixelNumberWidth - 1 || col % 10 == 0) {
            std::cout << '\r' << "Rendering " << 100 * col / pixelNumberWidth << "%";
            std::cout.flush();
        }
    }
    auto endTime = std::chrono::steady_clock::now();
    auto workTime = std::chrono::seconds(std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count());
    std::cout << "\nRendering finished\n"
        << "Total time " << workTime.count() / 60 << "m "
        << workTime.count() % 60 << "s\n"
        << "Statistics:\n"
        << "\tAttempts per ray " << double (stats.intersectionAtemptCount) / stats.rayNumber << '\n';
    return picture;
}

bool Scene::castRay(const Ray &ray, int restDepth, Point &intersection, Color &finalColor) {
    assert(restDepth >= 0); // todo: assert замедляет.
    stats.rayNumber++;
    double minSqrDistance = 0;  // Квадрат (для оптимизации) минимального расстояния до пересечения
    Object3d* obstacle = nullptr;  // Итератор соотвествующий объекту пересечения
    bool haveAny = false;
    // Перебираем все объекты сцены (пока в лоб), ищем ближайшее пересечение.
    for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
        stats.intersectionAtemptCount++;
        Point thisIntersection;
        if ((*iter)->intersectRay(ray, thisIntersection)) {
            double sqrDistance = (thisIntersection - ray.getOrigin()).lengthSquared();
            if ((!haveAny || sqrDistance < minSqrDistance) && !areDoubleEqual(sqrDistance, 0)) {
                haveAny = true;
                minSqrDistance = sqrDistance;
                intersection = thisIntersection;
                finalColor = (*iter)->getColor(thisIntersection);
                obstacle = *iter;
            }
        }
    }
    if (!haveAny) {
        return false;
    }
    if (restDepth == 0) {
        return true;
    }
    // Расчет освещенности.
    Color hsv = rgb2hsv(finalColor);
    double totalBrightness = backgroundIllumination;
    for (const LightSource* light : lights) {
        Vector3d intersectionToLight = light->getPoint() - intersection;
        Ray lightRay(intersection, intersectionToLight);
        double sqrDistanceToLight = intersectionToLight.lengthSquared();
        double dotProduct = Vector3d::dotProduct(obstacle->getNormal(intersection), lightRay.getDirection());
        // Отсекаем источники, находящиеся не перед поверхностью.
        if (dotProduct <= 0 || areDoubleEqual(sqrDistanceToLight, 0)) {
            continue;
        }
        Point obstacleHitPoint;
        Color obstacleColor;
        // Если луч не прервался перпятствием, то учтем его вклад в освещенность.
        if (!castRay(lightRay, 0, obstacleHitPoint, obstacleColor)) {
            double brightness = dotProduct / sqrDistanceToLight;
            totalBrightness += brightness * light->getBrightness();
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


