#ifndef RAYTRACER_RTREADER_H
#define RAYTRACER_RTREADER_H

#include <fstream>
#include <map>
#include "../Objects/Sphere.h"
#include "../Objects/Triangle3d.h"
#include "../Objects/PlaneQuadrangle.h"
#include "../LightSources/PointLight.h"

using std::string;

class RTReader {
public:
    static SceneData readScene(const std::string &path) {
        std::ifstream input(path);
        if (!input) {
            throw std::invalid_argument("Can't read file");
        }
        std::cout << "Reading " << path << std::endl;
        std::map<string, const Material *> materials;
        SceneData sceneData;
        string section;
        while (getNextWord(input, section)) {
            if (section == "viewport") {
                // Считаываем данные камеры.
                sceneData.camera = readCamera(input);
                continue;
            }
            if (section == "materials") {
                // Считаываем материалы.
                string entry;
                while (getNextWord(input, entry)) {
                    if (entry == "endmaterials") {
                        break;
                    }
                    if (entry == "entry") {
                        sceneData.addMaterial(readMaterials(input, materials));
                        continue;
                    }
                    showWarning(entry);
                }

                continue;
            }
            if (section == "lights") {
                // Считаываем источники освещения и нормализационную информацию.
                string entry;
                while (getNextWord(input, entry)) {
                    if (entry == "endlights") {
                        break;
                    }
                    if (entry == "point") {
                        sceneData.addLightSource(readPointLight(input));
                        continue;
                    }
                    if (entry == "reference") {
                        string property;
                        Point center;
                        double power = 0;
                        Color color = CL_WHITE;
                        while (getNextWord(input, property)) {
                            if (property == "endreference") {
                                break;
                            }
                            if (property == "distance") {
                                input >> sceneData.distance;
                                continue;
                            }
                            if (property == "power") {
                                input >> sceneData.power;
                                continue;
                            }
                            showWarning(property);
                        }
                    }
                }

                continue;
            }
            if (section == "geometry") {
                // Считаываем примитивы.
                string primitive;
                while (getNextWord(input, primitive)) {
                    if (primitive == "endgeometry") {
                        break;
                    }
                    if (primitive == "sphere") {
                        sceneData.addObject(readSphere(input, materials));
                        continue;
                    }
                    if (primitive == "triangle") {
                        sceneData.addObject(readTriangle(input, materials));
                        continue;
                    }
                    if (primitive == "quadrangle") {
                        sceneData.addObject(readQuadrangle(input, materials));
                        continue;
                    }
                    showWarning(primitive);
                }
                continue;
            }
            showWarning(section);
        }
        input.close();
        std::cout << "Read success: " << sceneData.objects.size() << " primitives" << std::endl;
        return sceneData;
    }

private:
    static bool getNextWord(std::ifstream &input, string &word) {
        do {
            if (input.eof()) {
                word.clear();
                return false;
            }
            input >> word;
            // Встретили комментарий, пропускаем строку.
            if (word.length() > 0 && word.front() == '#') {
                word.clear();
                while ((input.peek()) != '\n' && !input.eof()) {
                    input.ignore();
                }
            }
        } while (word.length() == 0);
        return true;
    }

    static Color readColor(std::ifstream &input) {
        unsigned r = 255, g = 255, b = 255;
        input >> r >> g >> b;
        return Color(((float) r) / 255, ((float) g) / 255, ((float) b) / 255);
    }

    static Sphere *readSphere(std::ifstream &input, const std::map<string, const Material *> &materials);

    static Triangle3d *readTriangle(std::ifstream &input, const std::map<string, const Material *> &materials);

    static PlaneQuadrangle *readQuadrangle(std::ifstream &input, const std::map<string, const Material *> &materials);

    static Material *readMaterials(std::ifstream &input, std::map<string, const Material *> &materials);

    static PointLight *readPointLight(std::ifstream &input) {
        string property;
        Point center;
        double power = 0;
        Color color = CL_WHITE;
        while (getNextWord(input, property)) {
            if (property == "endpoint") {
                break;
            }
            if (property == "coords") {
                input >> center;
                continue;
            }
            if (property == "power") {
                input >> power;
                continue;
            }
            if (property == "color") {
                color = readColor(input);
                continue;
            }
            showWarning(property);
        }
        return new PointLight(center, power, color);
    }

    static Camera readCamera(std::ifstream &input) {
        string property;
        Camera newCamera = DEFAULT_CAMERA;
        while (getNextWord(input, property)) {
            if (property == "endviewport") {
                break;
            }
            if (property == "origin") {
                input >> newCamera.viewPoint;
                continue;
            }
            if (property == "topleft") {
                input >> newCamera.topLeft;
                continue;
            }
            if (property == "bottomleft") {
                input >> newCamera.bottomLeft;
                continue;
            }
            if (property == "topright") {
                input >> newCamera.topRight;
                continue;
            }
            showWarning(property);
        }
        return newCamera;
    }

    static void showWarning(std::string unknownWord) {
        std::cerr << "[Warning] Unknown keyword '" << unknownWord << "'\n";
    }

};

Material *RTReader::readMaterials(std::ifstream &input, std::map<string, const Material *> &materials) {
    string property;
    string name;
    Color color = CL_WHITE;
    double reflectance = 0;
    double refractiveIndex = 1;
    double alpha = 1;
    while (getNextWord(input, property)) {
        if (property == "endentry") {
            break;
        }
        if (property == "name") {
            input >> name;
            continue;
        }
        if (property == "color") {
            color = readColor(input);
            continue;
        }
        if (property == "reflect") {
            input >> reflectance;
            continue;
        }
        if (property == "refract") {
            input >> refractiveIndex;
            continue;
        }
        if (property == "alpha") {
            input >> alpha;
            continue;
        }
        showWarning(property);
    }
    Material *newMaterial = new Material(color, reflectance, refractiveIndex, 1. - alpha);
    materials.insert({name, newMaterial});
    return newMaterial;
}

PlaneQuadrangle *RTReader::readQuadrangle(std::ifstream &input, const std::map<string, const Material *> &materials) {
    std::vector<Point> points(4);
    size_t currentPoint = 0;
    const Material *material = &NO_MATERIAL;
    string property;
    while (getNextWord(input, property)) {
        if (property == "endquadrangle") {
            break;
        }
        if (property == "vertex") {
            if (currentPoint <= 3) {
                input >> points[currentPoint];
                currentPoint++;
            } else {
                throw std::invalid_argument("Wrong quadrangle format");
            }
            continue;
        }
        if (property == "material") {
            string materialName;
            input >> materialName;
            auto materialIt = materials.find(materialName);
            if (materialIt != materials.end()) {
                material = materialIt->second;
            } else {
                throw std::invalid_argument("Material not found");
            }
            continue;
        }
        showWarning(property);
    }
    if (currentPoint != 4) {
        throw std::invalid_argument("Wrong quadrangle format");
    }
    return new PlaneQuadrangle({points[0], points[1], points[2], points[3]}, material);
}

Triangle3d *RTReader::readTriangle(std::ifstream &input, const std::map<string, const Material *> &materials) {
    std::vector<Point> points(3);
    size_t currentPoint = 0;
    const Material *material = &NO_MATERIAL;
    string property;
    while (getNextWord(input, property)) {
        if (property == "endtriangle") {
            break;
        }
        if (property == "vertex") {
            if (currentPoint <= 2) {
                input >> points[currentPoint];
                currentPoint++;
            }
            continue;
        }
        if (property == "material") {
            string materialName;
            input >> materialName;
            auto materialIt = materials.find(materialName);
            if (materialIt != materials.end()) {
                material = materialIt->second;
            } else {
                throw std::invalid_argument("Material not found");
            }
            continue;
        }
        showWarning(property);
    }
    if (currentPoint != 3) {
        throw std::invalid_argument("Wrong triangle format");
    }
    return new Triangle3d(points[0], points[1], points[2], material);
}

Sphere *RTReader::readSphere(std::ifstream &input, const std::map<string, const Material *> &materials) {
    Point center;
    double radius = 1;
    const Material *material = &NO_MATERIAL;
    string property;
    while (getNextWord(input, property)) {
        if (property == "endsphere") {
            break;
        }
        if (property == "coords") {
            input >> center;
            continue;
        }
        if (property == "radius") {
            input >> radius;
            continue;
        }
        if (property == "material") {
            string materialName;
            input >> materialName;
            auto materialIt = materials.find(materialName);
            if (materialIt != materials.end()) {
                material = materialIt->second;
            } else {
                throw std::invalid_argument("Material not found");
            }
            continue;
        }
        showWarning(property);
    }
    return new Sphere(center, radius, material);
}


#endif //RAYTRACER_RTREADER_H
