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
            Task task(topLeft,colVector, rowVector, 1, 1, picture.getPointerAt(col, row));
            taskQueue.push(task);
        }
    }
    taskQueue.close();
    for (std::thread &thread : threads) {
        thread.join();
    }

    if (properties.enableAntiAliasing) {
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
        total = total + computeIndirectIllumination(object, point);
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
                Point topLeft(colVector * col + rowVector * row + camera.topLeft);
                Task task(topLeft, colVector, rowVector,
                          (unsigned char) properties.antiAliasingWidth,
                          (unsigned char) properties.antiAliasingHeight,
                          smoothed.getPointerAt(col, row));
                taskQueue.push(task);
//                smoothed.setAt(col, row, CL_RED);//todo: use it to minimize color_precise parameter
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