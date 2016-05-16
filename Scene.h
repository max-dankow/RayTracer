#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <cmath>
#include <thread>
#include "Objects/Object3d.h"
#include "Geometry/Geometry.h"
#include "LightSources/LightSource.h"
#include "KdTree.h"
#include "SyncQueue/SyncQueue.h"
#include "PhotonMap.h"

struct Camera {

    Camera() { }

    Camera(const Point &viewPoint, const Point &topLeft,
           const Point &bottomLeft, const Point &topRight) :
            viewPoint(viewPoint), topLeft(topLeft),
            bottomLeft(bottomLeft), topRight(topRight) { }

    Point viewPoint, topLeft, bottomLeft, topRight;
};

static Camera DEFAULT_CAMERA(Point(0, 0, 12), Point(-2, 1.5, 10), Point(-2, -1.5, 10), Point(2, 1.5, 10));

struct SceneData {
    SceneData() : camera(DEFAULT_CAMERA) { }

    SceneData(const Camera &camera,
              std::vector<Object3d *> &&objects,
              std::vector<LightSource *> &&lights,
              std::vector<Material *> &&materials) :
            camera(camera),
            objects(std::move(objects)),
            lights(std::move(lights)),
            materials(std::move(materials)) { }

    SceneData(SceneData &&other) :
            camera(other.camera),
            objects(std::move(other.objects)),
            lights(std::move(other.lights)),
            materials(std::move(other.materials)) { }

    SceneData &operator=(SceneData &&other) {
        this->camera = other.camera;
        this->objects = std::move(other.objects);
        this->lights = std::move(other.lights);
        this->materials = std::move(other.materials);
        return *this;
    }

    SceneData& operator=(const SceneData&) = delete;
    SceneData(const SceneData&) = delete;

    void merge(SceneData &&other) {
        std::move(other.objects.begin(), other.objects.end(), std::back_inserter(this->objects));
        other.objects.clear();
        std::move(other.lights.begin(), other.lights.end(), std::back_inserter(this->lights));
        other.lights.clear();
        std::move(other.materials.begin(), other.materials.end(), std::back_inserter(this->materials));
        other.materials.clear();
    }

    void addObject(Object3d *&&object) {
        objects.push_back(object);
        object = nullptr;
    }

    void addLightSource(LightSource *&&lightSource) {
        lights.push_back(lightSource);
        lightSource = nullptr;
    }

    void addMaterial(Material *&&material) {
        materials.push_back(material);
        material = nullptr;
    }

    Camera camera;
    std::vector<Object3d *> objects;
    std::vector<LightSource *> lights;
    std::vector<Material *> materials;
};

struct SceneProperties {
    bool enableIllumination = true;
    bool enableReflection = true;
    bool enableRefraction = true;
    bool enableIndirectIllumination = false;
    bool enableAntiAliasing = false;
    size_t photonsNumber = 5000000;
    size_t antiAliasingWidth = 4, antiAliasingHeight = 4;
};

struct Task {
    Task() { }

    Task(const Point &topLeft, const Vector3d &colVector,
         const Vector3d &rowVector, unsigned char pixelNumberWidth,
         unsigned char pixelNumberHeight, Color *target) :
            topLeft(topLeft),
            colVector(colVector), rowVector(rowVector),
            pixelNumberWidth(pixelNumberWidth),
            pixelNumberHeight(pixelNumberHeight),
            target(target) { }

    Point topLeft;
    Vector3d colVector, rowVector;
    unsigned char pixelNumberWidth, pixelNumberHeight;
    Color *target;
};

class Scene {
public:

    Scene(SceneData &&sceneData,
          SceneProperties properties) :
            properties(properties),
            camera(sceneData.camera),
            objectList(std::move(sceneData.objects)),
            lights(std::move(sceneData.lights)) {
        this->objects = KdTree(std::vector<GeometricShape *>(objectList.begin(), objectList.end()));
        if (properties.enableIndirectIllumination) {
            this->photonMap = PhotonMap(this->lights, this->objects, properties.photonsNumber);
        }
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

    Picture render(size_t pixelNumberWidth, size_t pixelNumberHeight);

private:
    const Color computeRayColor(const Ray &ray, int restDepth);

    Color computeDiffuseColor(Object3d *object, const Point &point, const Ray &viewRay);

    Color computeReflectionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth);

    Color computeRefractionColor(Object3d *object, const Point &point, const Ray &viewRay, int restDepth);

    Color computeIndirectIllumination(Object3d *object, const Point &point) {
        auto normal = object->getNormal(point);
        auto KNN = photonMap.locatePhotons(point, 1, 500);
        if (KNN.empty()) {
            return CL_BLACK;
        }
        double r = 0, g = 0, b = 0;
        for (Photon *photon : KNN) {
            double k = -Vector3d::dotProduct(normal, photon->getRay().getDirection().normalize());
            if (k > 0) {
                r += photon->getColor().r * k;
                g += photon->getColor().g * k;
                b += photon->getColor().b * k;
            }
        }
//        std::cerr << r << ' ' << g << ' ' << b << ' ' << KNN.size() << '\n';
        double gatheringRadiusSqr = Vector3d(KNN.front()->getRay().getOrigin(), point).lengthSquared();
        double sphereArea = M_PI * gatheringRadiusSqr;
        return Color(r / sphereArea, g / sphereArea, b / sphereArea);
    }

    Color mixColors(const std::vector<Color> &neighbors) const {
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

    void worker(SyncQueue<std::vector<Task> > &tasks);

    bool isColorPreciseEnough(const std::vector<Color> &neighbors, Color &mixedColor) const {
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

    Color mixSubPixels(const Point &topLeft, const Vector3d &colVector, const Vector3d &rowVector,
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

    Picture smooth(const Picture &picture);

    static const int MAX_DEPTH = 10;
    const size_t THREAD_NUMBER = std::thread::hardware_concurrency();

    SceneProperties properties;

    Camera camera;
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
