#ifndef RAYTRACER_RTREADER_H
#define RAYTRACER_RTREADER_H

#include <fstream>
#include <map>
#include "SceneReader.h"
#include "../Objects/Sphere.h"
#include "../Objects/Triangle3d.h"
#include "../Objects/PlaneQuadrangle.h"

using std::string;

class RTReader : public SceneReader {

public:
    virtual SceneData readScene(const std::string &path) {
        std::ifstream input(path);
        if (!input) {
            throw std::invalid_argument("Can't read file");
        }
        std::map<string, const Material *> materials;
        SceneData sceneData;
        string section;
        while (getNextWord(input, section)) {
            if (section == "viewport") {
                // Считаываем данные камеры.
                std::cout << "CAM" << "\n";
                continue;
            }
            if (section == "materials") {
                // Считаываем материалы.
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
                    }
                    if (primitive == "triangle") {
                        sceneData.addObject(readTriangle(input, materials));
                    }
                    if (primitive == "quadrangle") {
                        sceneData.addObject(readQuadrangle(input, materials));
                    }
                }
                continue;
            }
        }
        input.close();
        return sceneData;
    }

private:
    bool getNextWord(std::ifstream &input, string &word) {
        do {
            if (input.eof()) {
                word.clear();
                return false;
            }
            input >> word;
            // Встретили комментарий, пропускаем строку.
            if (word.length() > 0 && word.front() == '#') {
                word.clear();
                while((input.peek()) != '\n' && !input.eof()) {
                    input.ignore();
                }
            }
        } while (word.length() == 0);
        return true;
    }

    Sphere* readSphere(std::ifstream &input, const std::map<string, const Material *> &materials) {
        Point center;
        double radius = 1;
        const Material* material = &NO_MATERIAL;
        string property;
        while (getNextWord(input, property)) {
            if (property == "endsphere") {
                break;
            }
            if (property == "coords") {
                input >> center;
            }
            if (property == "radius") {
                input >> radius;
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
            }
        }
        return new Sphere(center, radius, material);
    }

    Triangle3d* readTriangle(std::ifstream &input, const std::map<string, const Material *> &materials) {
        std::vector<Point> points(3);
        size_t currentPoint = 0;
        const Material* material = &NO_MATERIAL;
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
            }
        }
        if (currentPoint != 3) {
            throw std::invalid_argument("Wrong triangle format");
        }
        return new Triangle3d(points[0], points[1], points[2], material);
    }

    // todo: get rid of this copypaste
    PlaneQuadrangle* readQuadrangle(std::ifstream &input, const std::map<string, const Material *> &materials) {
        std::vector<Point> points(4);
        size_t currentPoint = 0;
        const Material* material = &NO_MATERIAL;
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
            }
        }
        if (currentPoint != 4) {
            throw std::invalid_argument("Wrong quadrangle format");
        }
        return new PlaneQuadrangle({points[0], points[1], points[2], points[3]}, material);
    }
};


#endif //RAYTRACER_RTREADER_H
