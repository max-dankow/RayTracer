#include <chrono>
#include <stack>
#include "Scene.h"

#define FIND_ALIASING

Picture Scene::render() {
    const size_t scale = 4;
    Vector3d colVector = Vector3d(screenBottomRight.x - screenTopLeft.x,
                                  0, screenBottomRight.z - screenTopLeft.z) / pixelNumberWidth;
    Vector3d rowVector = Vector3d(0, screenBottomRight.y - screenTopLeft.y, 0) / pixelNumberHeight;
    Picture picture(pixelNumberWidth, pixelNumberHeight);
    auto startTime = std::chrono::steady_clock::now();

    std::cout << "Start rendering scene...\n";
    for (size_t col = /*400*/0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0/*180*/; row < pixelNumberHeight; ++row) {
            // Смещаем 0.5 чтобы попасть в серединку пикселя.
            Point pixel(colVector * (0.5 + col) + rowVector * (0.5 + row) + screenTopLeft);
            Point intersection;
            auto color = computeRayColor(Ray(viewPoint, pixel - viewPoint), MAX_DEPTH, intersection);
            picture.setAt(col, row, color);
        }
        // Отображение прогресса.
        if (col == pixelNumberWidth - 1 || col % 5 == 0) {
            std::cout << '\r' << "Rendering " << 100 * col / pixelNumberWidth << "%";
            std::cout.flush();
        }
    }
    int count = 0;
    Picture newPic(pixelNumberWidth, pixelNumberHeight);
#ifdef FIND_ALIASING
    std::cout << "\nRunning Adaptive Anti Aliasing..." << "\n";
    // Выделение ступенчатых учатсков.
    for (size_t col = /*400*/1; col < pixelNumberWidth - 1; ++col) {
        for (size_t row = 1/*180*/; row < pixelNumberHeight -1; ++row) {
            Color ignoreColor;
            if (!isColorPreciseEnough({picture.getAt(col, row),
                                      picture.getAt(col + 1, row),
                                      picture.getAt(col, row + 1),
                                      picture.getAt(col - 1, row),
                                      picture.getAt(col, row - 1)}, ignoreColor)) {
                Point newTopLeft(colVector * col + rowVector * row + screenTopLeft);
                Point newBottomRight(colVector * (col + 1) + rowVector * (row + 1) + screenTopLeft);
                newPic.setAt(col, row, mixSubPixels(newTopLeft, newBottomRight, 0));
//                newPic.setAt(col, row, picture.getAt(col, row));
//                newPic.setAt(col, row, CL_RED);
                count++;
            } else {
                newPic.setAt(col, row, picture.getAt(col, row));
            }
            // Отображение прогресса.
            if (col == pixelNumberWidth - 1 || col % 20 == 0) {
                std::cout << '\r' << "Rendering " << 100 * col / pixelNumberWidth << "%";
                std::cout.flush();
            }
        }
    }
#endif  // FIND_ALIASING
    std::cout << "Bad are " << count << " of " << pixelNumberWidth * pixelNumberHeight << "\n";
    auto endTime = std::chrono::steady_clock::now();
    auto workTime = std::chrono::seconds(std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count());
    std::cout << "\nRendering finished\n"
    << "Total time " << workTime.count() / 60 << "m "
    << workTime.count() % 60 << "s\n";
    return newPic;
}

const Color Scene::computeRayColor(const Ray &ray, int restDepth, Point &hitPoint) {
//    static  int cuts = 0;
    if (ray.getPower() <= Ray::INSIGNIFICANT) {
//        std::cerr << ++cuts << " CUT\n";
        return CL_BLACK;
    }
//    if (restDepth <  5) {
//        std::cout << "!";
//        if (restDepth <  2) {
//            std::cout << "!!";
//        }
//        std::cout << "\n";
//    }
    if (restDepth <= 0) {
        return false;
    }
    Color finalColor;

    // Используя Kd дерево ищем пересечения со сценой.
    Object3d *obstacle = findObstacle(ray, hitPoint, finalColor);

    // Если не нашли пересечений.
    if (obstacle == nullptr) {
        return backgroundColor;
    }
    auto obstacleMaterial = obstacle->getMaterial();
    double surface = 1 - obstacleMaterial.reflectance - obstacleMaterial.transparency;
    surface = std::max(0., surface);//todo
    finalColor = computeDiffuseColor(obstacle, hitPoint, ray) * surface
                 + computeReflectionColor(obstacle, hitPoint, ray, restDepth) * obstacleMaterial.reflectance
                 + computeRefractionColor(obstacle, hitPoint, ray, restDepth) * obstacleMaterial.transparency;
    return finalColor;
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
    auto material = object->getMaterial();
    for (const LightSource *light : lights) {
        Vector3d lightDirection(point, light->getPoint());
        Ray lightRay(point, lightDirection);
        double sqrDistanceToLight = lightDirection.lengthSquared();

        // Если луч направлен с противоположной стороны от осточника относительно примитива, то отсекаем его.
        double dotNormalLight = Vector3d::dotProduct(object->getNormal(point), lightRay.getDirection());
        if (dotNormalLight * dotNormalRay >= 0 || Geometry::areDoubleEqual(sqrDistanceToLight, 0)) {
            continue;
        }
        lightRay.push();

        Point obstacleHitPoint;
        Color obstacleColor;
        Object3d *obstacle = findObstacle(lightRay, obstacleHitPoint, obstacleColor);
        // Если луч не прервался перпятствием, находящимся ДО источника, то учтем его вклад в освещенность.
        if (obstacle == nullptr
            || ((obstacleHitPoint - point).lengthSquared() > sqrDistanceToLight)
            || (obstacleHitPoint == light->getPoint())) {
            Vector3d lightReflectedDirection = reflectRay(lightDirection * -1, object->getNormal(point));
            double fong = 0;
            if (!Vector3d::isNullVector(lightReflectedDirection)) {
                double fongCos = std::max(-Vector3d::dotProduct(lightReflectedDirection, viewRay.getDirection()), 0.);
                fong = std::pow(fongCos, material.phongPower);
            }
            double brightness = dotNormalLight;
            totalBrightness += (brightness * material.lambert + fong * material.phong)
                               * light->getBrightness() / sqrDistanceToLight;
        }
    }
    return object->getColorAt(point) * totalBrightness;
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
    if (Geometry::areDoubleEqual(object->getMaterial().reflectance, 0)) {
        return CL_BLACK;
    }

    // Если имеет место отражение, продолжаем.
    Vector3d reflectionDirection = reflectRay(viewRay.getDirection(), object->getNormal(point));
    if (!Vector3d::isNullVector(reflectionDirection)) {
        Ray reflectionRay(point, reflectionDirection, (float) (viewRay.getPower() * object->getMaterial().reflectance));
        Point obstacleHitPoint;
        Color obstacleColor = computeRayColor(reflectionRay, restDepth - 1, obstacleHitPoint);
        return obstacleColor;
    }
    return CL_BLACK;
}

Color Scene::computeRefractionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth) {
    if (Geometry::areDoubleEqual(object->getMaterial().transparency, 0)) {
        return CL_BLACK;
    }
    Vector3d refractionDirection = refractRay(viewRay.getDirection(),
                                              object->getNormal(point),
                                              object->getMaterial().refractiveIndex);
    // Если имеет место преломление, продолжаем.
    if (!Vector3d::isNullVector(refractionDirection)) {
        Ray refractionRay(point, refractionDirection, (float) (viewRay.getPower() * object->getMaterial().transparency));
        refractionRay.push();
        Point obstacleHitPoint;
        Color obstacleColor = computeRayColor(refractionRay, restDepth - 1, obstacleHitPoint);
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
    return direction.normalize() * q - normal * (q * cosRayNormal + sqrt(1 - sinThetaSqr));
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
                && !Geometry::areDoubleEqual(sqrDistance, 0)) {
                minSqrDistance = sqrDistance;
                object->intersectRay(ray, thisIntersection);
                intersection = thisIntersection;
                obstacle = object;
                color = obstacle->getColorAt(intersection);
            }
        }
    }
    return obstacle;
}



