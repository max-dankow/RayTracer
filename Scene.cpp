#include <chrono>
#include <stack>
#include "Scene.h"

//#define ENABLE_ILLUMINATION
//#define ENABLE_REFLECTION
//#define ENABLE_REFRACTION

Picture Scene::render() {
    Vector3d colVector = Vector3d(screenBottomRight.x - screenTopLeft.x,
                                  0, screenBottomRight.z - screenTopLeft.z) / pixelNumberWidth;
    Vector3d rowVector = Vector3d(0, screenBottomRight.y - screenTopLeft.y, 0) / pixelNumberHeight;
    Picture picture(pixelNumberWidth, pixelNumberHeight);
    auto startTime = std::chrono::steady_clock::now();

    std::cout << "Start rendering scene...\n";
    for (size_t col = 0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0; row < pixelNumberHeight; ++row) {
            // Смещаем 0.5 чтобы попасть в серединку пикселя.
            Point pixel(colVector * (0.5 + col) + rowVector * (0.5 + row) + screenTopLeft);
            Point intersection;
            Color finalColor;
            if (!castRay(Ray(viewPoint, pixel - viewPoint), 10, intersection, finalColor)) {
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
    << workTime.count() % 60 << "s\n";
    return picture;
}

bool Scene::castRay(const Ray &ray, int restDepth, Point &hitPoint, Color &finalColor) {
    assert(restDepth >= 0);

    // Используя Kd дерево ищем пересечения со сценой.
    Object3d *obstacle = findObstacle(ray, hitPoint, finalColor);

    // Если не нашли пересечений.
    if (obstacle == nullptr) {
        return false;
    }
    if (restDepth == 0) {
        return true;
    }
    finalColor = computeDiffuseColor(obstacle, hitPoint, ray) * (1 - obstacle->getReflectance())
                 + computeReflectionColor(obstacle, hitPoint, ray, restDepth) * obstacle->getReflectance();
    return true;
}

Object3d *Scene::findObstacle(const Ray &ray, Point &hitPoint, Color &obstacleColor) {
    Object3d *obstacle = nullptr;
    std::stack<KdNode *> stack;
    KdNode *node = objects.getRoot();
    while (true) {
        if (node->getIsLeaf()) {
            // пора считать пересечения.
            obstacle = checkIntersection(ray, node->getObjects(), hitPoint, obstacleColor);
            // Если в этом листе нет пересечений, то нужно вернуться на уровень выше.
            if (obstacle != nullptr && node->getBox().containsPoint(hitPoint)) {
                // Пересечение успешно найдено.
                break;
            }
        } else {
            auto leftBox = node->getBox();
            leftBox.maxCorner[node->getSplitAxis()] = node->getSplitPoint();

            auto rightBox = node->getBox();
            rightBox.minCorner[node->getSplitAxis()] = node->getSplitPoint();

            double t_left = leftBox.intersectRay(ray);
            double t_right = rightBox.intersectRay(ray);

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
        }

        // Если ничего не найдено то вернуться на уровень выше.
        if (stack.empty()) {
            break;
        }
        node = stack.top();
        stack.pop();
    }
    return obstacle;
}

Color Scene::computeDiffuseColor(Object3d *object, const Point &point, const Ray &viewRay) {
    double totalBrightness = backgroundIllumination;
    double dotNormalRay = Vector3d::dotProduct(object->getNormal(point), viewRay.getDirection());
    for (const LightSource *light : lights) {
        Vector3d lightDirection(point, light->getPoint());
        Ray lightRay(point, lightDirection);
        double sqrDistanceToLight = lightDirection.lengthSquared();

        // Если луч направлен с противоположной стороны от осточника относительно примитива, то отсекаем его.
        double dotNormalLight = Vector3d::dotProduct(object->getNormal(point), lightRay.getDirection());
        if (dotNormalLight * dotNormalRay >= 0 || Geometry::areDoubleEqual(sqrDistanceToLight, 0)) {
            continue;
        }

        Point obstacleHitPoint;
        Color obstacleColor;
        Object3d *obstacle = findObstacle(lightRay, obstacleHitPoint, obstacleColor);
        // Если луч не прервался перпятствием, находящимся ДО источника, то учтем его вклад в освещенность.
        if (obstacle == nullptr
            || ((obstacleHitPoint - point).lengthSquared() > sqrDistanceToLight)
            || (obstacleHitPoint == light->getPoint())) {
            double brightness = dotNormalLight / sqrDistanceToLight;
            totalBrightness += brightness * light->getBrightness();
        }
    }
    return object->getColor(point) * totalBrightness;
}

Vector3d Scene::reflectRay(const Vector3d &direction, const Vector3d &normal) {
    Vector3d normalizedDirection = direction.normalize();
    double cosRayNormal = Vector3d::dotProduct(normal, normalizedDirection);
    // Если луч направлен с лицевой стороны.
    if (cosRayNormal <= 0) {
        cosRayNormal = fabs(cosRayNormal);
        Vector3d reflectionDirection(((normal * cosRayNormal) + normalizedDirection) * 2 - normalizedDirection);
        assert(Geometry::areDoubleEqual(cosRayNormal, Vector3d::dotProduct(reflectionDirection, normal)));
        return reflectionDirection;
    }
    return Vector3d::nullVector();
}

Color Scene::computeReflectionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth) {
    if (Geometry::areDoubleEqual(object->getReflectance(), 0)) {
        return CL_BLACK;
    }
    // Если имеет место отражение, продолжаем.
    Vector3d reflectionDirection = reflectRay(viewRay.getDirection(), object->getNormal(point));
    if (!Vector3d::isNullVector(reflectionDirection)) {
        Ray reflectionRay(point, reflectionDirection);
        Point obstacleHitPoint;
        Color obstacleColor = backgroundColor;
        castRay(reflectionRay, restDepth - 1, obstacleHitPoint, obstacleColor);
        return obstacleColor;
    }
    return CL_BLACK;
}

Color Scene::computeRefractionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth) {
    Vector3d refractionDirection = refractRay(viewRay.getDirection(),
                                              object->getNormal(point),
                                              object->getRefractiveCoef());
    // Если имеет место преломление, продолжаем.
    if (!Vector3d::isNullVector(refractionDirection)) {
        Ray refractionRay(point, refractionDirection);
        refractionRay.push();
        Point obstacleHitPoint;
        Color obstacleColor = backgroundColor;
        castRay(refractionRay, restDepth - 1, obstacleHitPoint, obstacleColor);
        return obstacleColor;
    }
    return CL_BLACK;
}

Vector3d Scene::refractRay(const Vector3d &direction, const Vector3d &normal, double q) {
    if (Geometry::areDoubleEqual(q, 0)) {
        return Vector3d(0, 0, 0);
    }
    double cosRayNormal = Vector3d::dotProduct(normal, direction.normalize());
    // Если луч направлен с изнаночной стороны объекта, то нужно использовать 1 / q.
    if (cosRayNormal > 0) {
        return refractRay(direction, normal * -1, 1 / q);
    }
    double sinThetaSqr = q * q * (1 - cosRayNormal * cosRayNormal);
    // Полное внутреннее отражение.
    if (sinThetaSqr > 1) {
        return Vector3d(0, 0, 0);
    }
    return direction * q + normal * (q * cosRayNormal - sqrt(1 - sinThetaSqr));
}


Object3d *Scene::checkIntersection(const Ray &ray,
                                   const std::vector<Object3d *> objectList,
                                   Point &intersection,
                                   Color &color) {
    double minSqrDistance = 0;  // Квадрат (для оптимизации) минимального расстояния до пересечения
    Object3d *obstacle = nullptr;
    for (Object3d *object : objectList) {
        Point thisIntersection;
        if (object->intersectRay(ray, thisIntersection)) {
            double sqrDistance = (thisIntersection - ray.getOrigin()).lengthSquared();
            if ((obstacle == nullptr || sqrDistance < minSqrDistance)
                && !Geometry::areDoubleEqual(sqrDistance, 0)) { //todo : push ray or this?
                minSqrDistance = sqrDistance;
                object->intersectRay(ray, thisIntersection);
                intersection = thisIntersection;
                obstacle = object;
                color = obstacle->getColor(intersection);
            }
        }
    }
    return obstacle;
}



