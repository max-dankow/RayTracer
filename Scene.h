#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <cmath>
#include <list>
#include <memory>
#include <thread>
#include "Objects/Object3d.h"
#include "Geometry/Geometry.h"
#include "Painter/Painter.h"
#include "LightSources/LightSource.h"
#include "KdTree.h"
#include "SyncQueue/SyncQueue.h"
#include "PhotonMap.h"

struct Task {
    enum TaskType {
        Trace,
        AA
    };
    Task()=default;


    Task(TaskType type, size_t col, size_t row, const Point &point, const Point &topLeft = Point(), const Point &bottomRight=Point()) :
            type(type), col(col),row(row),point(point),topLeft(topLeft),bottomRight(bottomRight) { }

    TaskType type;
    size_t col, row;
    Point point;
    Point topLeft, bottomRight;
};

class Scene {
public:

    Scene(const Point &viewPoint,
          const Point &screenTopLeft,
          const Point &screenBottomRight,
          size_t pixelNumberWidth,
          size_t pixelNumberHeight,
          std::vector<Object3d *> &&objects,
          std::vector<LightSource *> &&lights, size_t photonsNumber = 1000000) :
            viewPoint(viewPoint),
            screenTopLeft(screenTopLeft),
            screenBottomRight(screenBottomRight),
            pixelNumberWidth(pixelNumberWidth),
            pixelNumberHeight(pixelNumberHeight),
            objectList(std::move(objects)),
            lights(std::move(lights)) {
        this->objects = KdTree(std::vector<GeometricShape *>(objectList.begin(), objectList.end()));
        this->photonMap = PhotonMap(this->lights, this->objects, photonsNumber);
    }

    ~Scene() {
        std::cout << "Destructing Scene\n";
        for (LightSource *light : lights) {
            delete light;
        }
        for (Object3d *object : objectList) {
            delete object;
        }
    }

    Picture render();

    static std::vector<Object3d *> &mergeObjects(std::vector<Object3d *> &base,
                                                 std::vector<Object3d *> &&other) {
        std::move(other.begin(), other.end(), std::back_inserter(base));
        other.clear();
        return base;
    }

private:
    const Color computeRayColor(const Ray &ray, int restDepth);

//    Object3d *checkIntersection(const Ray &ray,
//                                const std::vector<Object3d *> &objectList,
//                                Point &intersection, Color &color);
//
//    Object3d *findObstacle(const Ray &ray, Point &hitPoint, Color &obstacleColor);

    Color computeDiffuseColor(Object3d *object, const Point &point, const Ray &viewRay);

    Color computeReflectionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth);

    Color computeRefractionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth);

    Color computeIndirectIllumination(Object3d *object, const Point &point) {
        auto KNN = photonMap.locatePhotons(point, 1, 1000);
        if (KNN.empty()) {
            return CL_BLACK;
        }
        double r=0, g=0, b=0;
        for (Photon *photon : KNN) {
            r += photon->getColor().r;
            g += photon->getColor().g;
            b += photon->getColor().b;
        }
//        std::cerr << r << ' ' << g << ' ' << b << ' ' << KNN.size() << '\n';
        double gatheringRadiusSqr = Vector3d(KNN.front()->getRay().getOrigin(), point).lengthSquared();
        double sphereArea = 4. * M_PI * gatheringRadiusSqr;
        return Color(r / sphereArea, g / sphereArea, b / sphereArea);
    }

    Color mixColors(const std::vector<Color> &neighbors) {
        double r = 0, g = 0, b = 0;
        for (Color color : neighbors) {
            r += color.r;
            g += color.g;
            b += color.b;
        }
        r /= neighbors.size();
        g /= neighbors.size();
        b /= neighbors.size();
        return Color(r, g, b);
    }

    void worker(SyncQueue<std::vector<Task> > &tasks, Picture &picture);

    bool isColorPreciseEnough(const std::vector<Color> &neighbors, Color &mixedColor) {
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

    Color mixSubPixels(const Point &topLeft, const Point &bottomRight, int depth) {
        const size_t AAScale = 4;
        Vector3d colVector = Vector3d(bottomRight.x - topLeft.x,
                                      0, bottomRight.z - topLeft.z) / AAScale;
        Vector3d rowVector = Vector3d(0, bottomRight.y - topLeft.y, 0) / AAScale;
//        Picture picture(AAScale, AAScale);
        std::vector<Color> colors;
//        Color picture[2][2];

        for (size_t col = 0; col < AAScale; ++col) {
            for (size_t row = 0; row < AAScale; ++row) {
                // Смещаем 0.5 чтобы попасть в серединку пикселя.
                Point pixel(colVector * (0.5 + col) + rowVector * (0.5 + row) + topLeft);
                auto color = computeRayColor(Ray(viewPoint, pixel - viewPoint), MAX_DEPTH);
//                picture.setAt(col, row,color);
                colors.push_back(color);
            }
        }
//        Color localColor;
//        bool enough = isColorPreciseEnough(picture.getColorMap(), localColor);
//
//        if (!enough && (depth < 0)) {
//
//            for (size_t col = 0; col < AAScale; ++col) {
//                for (size_t row = 0; row < AAScale; ++row) {
//                    Point newTopLeft(colVector * col + rowVector * row + topLeft);
//                    Point newBottomRight(colVector * (col + 1) + rowVector * (row + 1) + topLeft);
//                    picture.setAt(col, row, mixSubPixels(newTopLeft, newBottomRight, depth + 1));
//                }
//            }
//            localColor = mixColors(picture.getColorMap());
//        }
        return mixColors(colors);
    }


    static const int MAX_DEPTH = 10;
    const size_t THREAD_NUMBER = std::thread::hardware_concurrency();

    // Точка камеры.
    Point viewPoint;

    // Верхний левый и правый нижний угол экрана.
    // Считаем что стороны экрана параллельны осям.
    // todo: нормальный формат камеры
    Point screenTopLeft, screenBottomRight;

    size_t pixelNumberWidth, pixelNumberHeight;
    Color backgroundColor = CL_BLACK;
    double backgroundIllumination = 0;

    // Kd дерево указателей на все объекты сцены, хранимые в куче.
    KdTree objects;
    PhotonMap photonMap;
    std::vector<Object3d *> objectList;
    // Указатели на все источники освещения, так же хранимые в куче.
    std::vector<LightSource *> lights;
};


#endif //RAYTRACER_SCENE_H
