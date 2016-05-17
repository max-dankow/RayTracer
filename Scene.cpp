#include <chrono>
#include <thread>
#include "Scene.h"

Picture Scene::render(size_t pixelNumberWidth, size_t pixelNumberHeight) {
    Vector3d colVector = Vector3d(camera.topLeft, camera.topRight) / pixelNumberWidth;
    Vector3d rowVector = Vector3d(camera.topLeft, camera.bottomLeft) / pixelNumberHeight;
    Picture picture(pixelNumberWidth, pixelNumberHeight);

    auto startTime = std::chrono::steady_clock::now();

    SyncQueue<std::vector<Task> > taskQueue;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < THREAD_NUMBER; ++i) {
        threads.emplace_back(&Scene::worker, this, std::ref(taskQueue));
    }

    std::cout << "Start rendering scene "
    << pixelNumberWidth << "x" << pixelNumberHeight
    << " with " << THREAD_NUMBER << " threads...\n";

    for (size_t col = 0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0; row < pixelNumberHeight; ++row) {
            Point topLeft(colVector * col + rowVector * row + camera.topLeft);
            Task task(topLeft, colVector, rowVector, 1, 1, picture.getPointerAt(col, row));
            taskQueue.push(task);
        }
    }
    taskQueue.close();
    for (std::thread &thread : threads) {
        thread.join();
    }

    if (properties.enableAntiAliasing || properties.highlightAliasing) {
        picture = smooth(picture);
    }

    auto endTime = std::chrono::steady_clock::now();
    auto workTime = std::chrono::seconds(std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count());

    std::cout << "Rendering finished\n"
    << "Total time " << workTime.count() / 60 << "m "
    << workTime.count() % 60 << "s\n";

    return picture;
}

const Color Scene::computeRayColor(const Ray &ray, int restDepth) {
    if (ray.getPower() <= Ray::INSIGNIFICANT) {
        return CL_BLACK;
    }

    if (restDepth <= 0) {
        return CL_BLACK;
    }

    // Используя Kd дерево ищем пересечения со сценой.
    Point hitPoint;
    Object3d *obstacle = dynamic_cast<Object3d *> (objects.findObstacle(ray, hitPoint));

    // Если не нашли пересечений.
    if (obstacle == nullptr) {
        return backgroundColor;
    }

    auto obstacleMaterial = obstacle->getMaterial();
    auto obstacleColor = obstacle->getColorAt(hitPoint);
    if (!properties.enableIllumination) {
        return obstacleColor;
    }

    double surface = 1 - obstacleMaterial->reflectance - obstacleMaterial->transparency;
    surface = std::max(0., surface);//todo: a + b + c = 1; diffuse is also property
    Color finalColor = computeDiffuseColor(obstacle, hitPoint, ray) * surface;

    if (properties.enableReflection) {
        finalColor += computeReflectionColor(obstacle, hitPoint, ray, restDepth)
                      * obstacleMaterial->reflectance;
    }

    if (properties.enableRefraction) {
        finalColor += (computeRefractionColor(obstacle, hitPoint, ray, restDepth) * obstacleColor)
                      * obstacleMaterial->transparency;
    }

    return finalColor;
}

Color Scene::computeIndirectIllumination(Object3d *object, const Point &point, const Ray &viewRay) {
    auto normal = object->getNormal(point);
    auto KNN = photonMap.locatePhotons(point, 1, 500);
    if (KNN.empty()) {
        return CL_BLACK;
    }
    double r = 0, g = 0, b = 0;
    double viewRayCos = Vector3d::dotProduct(normal, viewRay.getDirection());
    for (Photon *photon : KNN) {
        double photonCos = Vector3d::dotProduct(normal, photon->getRay().getDirection().normalize());
        if (photonCos * viewRayCos > 0) {
            photonCos = std::abs(photonCos);
            r += photon->getColor().r * photonCos;
            g += photon->getColor().g * photonCos;
            b += photon->getColor().b * photonCos;
        }
    }
    double gatheringRadiusSqr = Vector3d(KNN.front()->getRay().getOrigin(), point).lengthSquared();
    double sphereArea = M_PI * gatheringRadiusSqr;
    return Color(r / sphereArea, g / sphereArea, b / sphereArea);
}

Color Scene::computeDiffuseColor(Object3d *object, const Point &point, const Ray &viewRay) {
//    double totalR = 0;
//    double totalG = 0;
//    double totalB = 0; // todo: extend Color to deal with such hyper colors
    Color total(CL_BLACK);
    double dotNormalRay = Vector3d::dotProduct(object->getNormal(point), viewRay.getDirection());
    auto material = object->getMaterial();
    for (const LightSource *light : lights) {
        Vector3d lightDirection(point, light->getPoint());
        Ray lightRay(point, lightDirection);
        double sqrDistanceToLight = lightDirection.lengthSquared();

        // Если луч направлен с противоположной стороны от осточника относительно примитива, то отсекаем его,
        // или если смотрим с изнаночной стороны.
        double dotNormalLight = Vector3d::dotProduct(object->getNormal(point), lightRay.getDirection());
        if (dotNormalLight <= 0 || dotNormalRay >= 0 || Geometry::areDoubleEqual(sqrDistanceToLight, 0)) {
            continue;
        }
        lightRay.push();

        Point obstacleHitPoint;
        Object3d *obstacle = dynamic_cast<Object3d *> (objects.findObstacle(lightRay, obstacleHitPoint));
        // Если луч не прервался перпятствием, находящимся ДО источника, то учтем его вклад в освещенность.
        if (obstacle == nullptr
            || ((obstacleHitPoint - point).lengthSquared() > sqrDistanceToLight)
            || (obstacleHitPoint == light->getPoint())) {
            Vector3d lightReflectedDirection = lightRay.reflectRay(object->getNormal(point));
            double fong = 0;
            if (!Vector3d::isNullVector(lightReflectedDirection)) {
                double fongCos = std::max(-Vector3d::dotProduct(lightReflectedDirection, viewRay.getDirection()), 0.);
                fong = std::pow(fongCos, material->phongPower);
            }
            double brightness = dotNormalLight;
            total = total + light->getColor()
                            * ((brightness * material->lambert + fong * material->phong) * light->getBrightness()
                               / sqrDistanceToLight);
        }
    }

    if (properties.enableIndirectIllumination) {
        total = total + computeIndirectIllumination(object, point, viewRay);
    }

    return object->getColorAt(point) * total;
}

Color Scene::computeReflectionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth) {
    if (Geometry::areDoubleEqual(object->getMaterial()->reflectance, 0)) {
        return CL_BLACK;
    }

    // Если имеет место отражение, продолжаем.
    Vector3d reflectionDirection = viewRay.reflectRay(object->getNormal(point));
    if (!Vector3d::isNullVector(reflectionDirection)) {
        Ray reflectionRay(point, reflectionDirection,
                          (float) (viewRay.getPower() * object->getMaterial()->reflectance));
        Color obstacleColor = computeRayColor(reflectionRay, restDepth - 1);
        return obstacleColor;
    }
    return CL_BLACK;
}

Color Scene::computeRefractionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth) {
    if (Geometry::areDoubleEqual(object->getMaterial()->transparency, 0)) {
        return CL_BLACK;
    }
    Vector3d refractionDirection =
            viewRay.refractRay(object->getNormal(point), object->getMaterial()->refractiveIndex);
    // Если имеет место преломление, продолжаем.
    if (!Vector3d::isNullVector(refractionDirection)) {
        Ray refractionRay(point, refractionDirection,
                          (float) (viewRay.getPower() * object->getMaterial()->transparency));
        refractionRay.push();
        Color obstacleColor = computeRayColor(refractionRay, restDepth - 1);
        return obstacleColor;
    }
    return CL_BLACK;
}

Color Scene::mixSubPixels(const Point &topLeft, const Vector3d &colVector, const Vector3d &rowVector,
                          size_t pixelNumberWidth, size_t pixelNumberHeight) {
    assert(pixelNumberWidth > 0 && pixelNumberHeight > 0);
    std::vector<Color> colors;
    auto colSubVector = colVector * (1. / pixelNumberWidth);
    auto rowSubVector = rowVector * (1. / pixelNumberHeight);
    colors.reserve(pixelNumberWidth * pixelNumberWidth);

    for (size_t col = 0; col < pixelNumberWidth; ++col) {
        for (size_t row = 0; row < pixelNumberHeight; ++row) {
            // Смещаем 0.5 чтобы попасть в серединку пикселя.
            Point pixel(colSubVector * (0.5 + col) + rowSubVector * (0.5 + row) + topLeft);
            auto color = computeRayColor(Ray(camera.viewPoint, pixel - camera.viewPoint), MAX_DEPTH);
            colors.push_back(color);
        }
    }
    return mixColors(colors);
}

bool Scene::isColorPreciseEnough(const std::vector<Color> &neighbors, Color &mixedColor) const {
    const double COLOR_PRECISION = 0.05;
    mixedColor = mixColors(neighbors);

    for (Color color : neighbors) {
        double distance = (mixedColor.r - color.r) * (mixedColor.r - color.r)
                          + (mixedColor.g - color.g) * (mixedColor.g - color.g)
                          + (mixedColor.b - color.b) * (mixedColor.b - color.b);
        if (distance > COLOR_PRECISION * COLOR_PRECISION) {
            return false;
        }
    }
    return true;
}

void Scene::worker(SyncQueue<std::vector<Task> > &tasks) {
    while (true) {
        Task result;
        if ((tasks.popOrWait()).some(result)) {
            auto color = mixSubPixels(result.topLeft, result.colVector, result.rowVector,
                                      result.pixelNumberWidth, result.pixelNumberHeight);
            *(result.target) = color;
            //todo: Отображение прогресса.
//            if (n++ % 1000 == 0) {
//                std::cout << '\r' << "Rendering " << 100 * n / (pixelNumberWidth * pixelNumberHeight) << "%";
//                std::cout.flush();
//            }
        } else {
            if (tasks.isClosed()) {
                break;
            }
        }
    }
}

Picture Scene::smooth(const Picture &picture) {
    size_t pixelNumberWidth = picture.getWidth();
    size_t pixelNumberHeight = picture.getHeight();
    Picture smoothed(pixelNumberWidth, pixelNumberHeight);

    Vector3d colVector = Vector3d(camera.topLeft, camera.topRight) / pixelNumberWidth;
    Vector3d rowVector = Vector3d(camera.topLeft, camera.bottomLeft) / pixelNumberHeight;

    SyncQueue<std::vector<Task> > taskQueue;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < THREAD_NUMBER; ++i) {
        threads.emplace_back(&Scene::worker, this, std::ref(taskQueue));
    }

    std::cout << "Running Adaptive AntiAliasing x"
    << properties.antiAliasingWidth * properties.antiAliasingHeight
    << " with " << THREAD_NUMBER << " threads...\n";

    // Выделение ступенчатых учатсков.
    for (size_t col = 1; col < pixelNumberWidth - 1; ++col) {
        for (size_t row = 1; row < pixelNumberHeight - 1; ++row) {
            Color ignoreColor;
            if (!isColorPreciseEnough({picture.getAt(col, row),
                                       picture.getAt(col + 1, row),
                                       picture.getAt(col, row + 1),
                                       picture.getAt(col - 1, row),
                                       picture.getAt(col, row - 1)}, ignoreColor)) {
                if (properties.highlightAliasing) {
                    smoothed.setAt(col, row, CL_RED);
                } else {
                    Point topLeft(colVector * col + rowVector * row + camera.topLeft);
                    Task task(topLeft, colVector, rowVector,
                              (unsigned char) properties.antiAliasingWidth,
                              (unsigned char) properties.antiAliasingHeight,
                              smoothed.getPointerAt(col, row));
                    taskQueue.push(task);
                }
            } else {
                smoothed.setAt(col, row, picture.getAt(col, row));
            }
        }
    }
    taskQueue.close();
    for (std::thread &thread : threads) {
        thread.join();
    }
    return smoothed;
}
