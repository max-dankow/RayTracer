#include <chrono>
#include <stack>
#include "Scene.h"

#define ENABLE_ILLUMINATION
#define ENABLE_REFLECTION

Picture Scene::render() {
    Vector3d colVector = Vector3d(screenBottomRight.x - screenTopLeft.x,
                                  0, screenBottomRight.z - screenTopLeft.z) / pixelNumberWidth;
    Vector3d rowVector = Vector3d(0, screenBottomRight.y - screenTopLeft.y, 0) / pixelNumberHeight;
    Picture picture(pixelNumberWidth, pixelNumberHeight);
    auto startTime = std::chrono::steady_clock::now();

    std::cout << "Start rendering scene...\n";
    stats.intersectionAttemptCount = 0;
    stats.rayNumber = 0;
    for (size_t col = 0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0; row < pixelNumberHeight; ++row) {
            // Смещаем 0.5 чтобы попасть в серединку пикселя.
            Point pixel(colVector * (0.5 + col) + rowVector * (0.5 + row) + screenTopLeft);
            Point intersection;
            Color finalColor;
            if (!castRay(Ray(viewPoint, pixel - viewPoint), 2, intersection, finalColor)) {
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
        << "\tAttempts per ray " << double (stats.intersectionAttemptCount) / stats.rayNumber << '\n';
    return picture;
}

bool Scene::castRay(const Ray &ray, int restDepth, Point &intersection, Color &finalColor) {
    assert(restDepth >= 0); // todo: assert замедляет.
    stats.rayNumber++;

    // Используя Kd дерево ищем пересечения со сценой.
    Object3d* obstacle = nullptr;
    std::stack<KdNode*> stack;
    KdNode* node = objects.getRoot();
    while(true) { // todo: на втором уровне вложенности начнутся большие проблемы
        if (node->getIsLeaf()) {
            // пора считать пересечения.
            obstacle = checkIntersection(ray, intersection, node->getObjects(), finalColor);
            // Если в этом листе нет пересечений, то нужно вернуться на уровень выше.
            if (obstacle == nullptr || !node->getBox().containsPoint(intersection)) {
                // Пересечение вообще нигде не найдено.
                if (stack.empty()) {
                    break;
                }
                node = stack.top();
                stack.pop();
                continue;
            } else {
                // Пересечение успешно найдено.
                break;
            }
        }
        auto leftBox = node->getBox();
        leftBox.maxCorner[node->getSplitAxis()] = node->getSplitPoint();

        auto rightBox = node->getBox();
        rightBox.minCorner[node->getSplitAxis()] = node->getSplitPoint();

        double t_left = leftBox.intersectRay(ray);
        double t_right = rightBox.intersectRay(ray);

//        std::cout << ray.getPointAt(t_left) << " = LEFT\n";
//        std::cout << ray.getPointAt(t_right) << " = RIGHT\n";

        // Сначала пересекается leftBox.
        if (t_left <= t_right) {
            auto old = node;
            if (t_left != std::numeric_limits<double>::infinity()) {
                node = old->getLeftPointer();
                if (t_right != std::numeric_limits<double>::infinity()) {// todo : method isNotInf
                    stack.push(old->getRightPointer());
                }
                continue;
            }
        }
        // Сначала пересекается rightBox.
        if (t_right < t_left) {
            auto old = node;
            if (t_right != std::numeric_limits<double>::infinity()) {
                node = old->getRightPointer();
                if (t_left != std::numeric_limits<double>::infinity()) {
                    stack.push(old->getLeftPointer());
                }
                continue;
            }
        }

        if (stack.empty()) {
            break;
        }
        node = stack.top();
        stack.pop();
    }
    // Если не нашли пересечений.
    if (obstacle == nullptr) {
        return false;
    }
    if (restDepth == 0) {
        return true;
    }

#ifdef ENABLE_ILLUMINATION

    // Расчет освещенности.
    Color hsv = rgb2hsv(finalColor);
    double totalBrightness = backgroundIllumination;
    for (const LightSource* light : lights) {
        Vector3d intersectionToLight = light->getPoint() - intersection;
        Ray lightRay(intersection, intersectionToLight);
        double sqrDistanceToLight = intersectionToLight.lengthSquared();
        double dotProduct = Vector3d::dotProduct(obstacle->getNormal(intersection), lightRay.getDirection());
        // Отсекаем источники, находящиеся не перед поверхностью.
        if (dotProduct <= 0 || Geometry::areDoubleEqual(sqrDistanceToLight, 0)) {
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
#endif

#ifdef ENABLE_REFLECTION
    double cosRayNormal = Vector3d::dotProduct(obstacle->getNormal(intersection), ray.getDirection().normalize());
    // Если луч направлен с лицевой стороны.
    if (cosRayNormal <= 0) {
        cosRayNormal = fabs(cosRayNormal);
        Vector3d reflectionDirection(((obstacle->getNormal(intersection) * cosRayNormal) + ray.getDirection()) * 2 - ray.getDirection());
        assert(Geometry::areDoubleEqual(cosRayNormal, Vector3d::dotProduct(reflectionDirection, obstacle->getNormal(intersection))));
        Ray reflectionRay(intersection, reflectionDirection);
        Point obstacleHitPoint;
        Color obstacleColor;
        if (castRay(reflectionRay, restDepth - 1, obstacleHitPoint, obstacleColor)) {
            // Мешаем цвета.
            double reflectance = obstacle->getReflectance();
            finalColor = obstacleColor * reflectance + finalColor * (1 - reflectance);
        }
    }
#endif

    return true;
}

Object3d *Scene::checkIntersection(const Ray &ray, Point &intersection, const std::vector<Object3d *> objectList,
                                   Color &color) {
    double minSqrDistance = 0;  // Квадрат (для оптимизации) минимального расстояния до пересечения
    Object3d* obstacle = nullptr;  // Итератор соотвествующий объекту пересечения
    bool haveAny = false;
    for (auto iter = objectList.begin(); iter != objectList.end(); ++iter) {
        stats.intersectionAttemptCount++;
        Point thisIntersection;
        if ((*iter)->intersectRay(ray, thisIntersection)) {
            double sqrDistance = (thisIntersection - ray.getOrigin()).lengthSquared();
            if ((!haveAny || sqrDistance < minSqrDistance) && !Geometry::areDoubleEqual(sqrDistance, 0)) {
                haveAny = true;
                minSqrDistance = sqrDistance;
                intersection = thisIntersection;
                obstacle = *iter;
                color = obstacle->getColor(intersection);
            }
        }
    }
    return obstacle;
}
//
//void Scene::emplaceObject(Object3d *object) {
//    objects.emplace_back(object);
//}


