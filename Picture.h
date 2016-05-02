#ifndef RAYTRACER_PICTURE_H
#define RAYTRACER_PICTURE_H

#include <assert.h>
#include <cstdlib>
#include <vector>
#include <limits>
#include <iostream>

// todo: перейти к одному байту - 24 -> 3; Тогда Picture 800x600 11Mb -> 1.4mb
// Представляет цвет в формате RGB каждая компонента - насыщенность от 0 до 1.
struct Color {
    Color(double r = 0, double g = 0, double b = 0) : r(r), g(g), b(b) {
//        assert(r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1);
    }

    Color operator*(double k) const {
        return Color(std::min(r * k, 1.), std::min(g * k, 1.), std::min(b * k, 1.));
    }

    Color operator+(const Color &other) const {
        return Color(std::min(this->r + other.r, 1.),
                     std::min(this->g + other.g, 1.),
                     std::min(this->b + other.b, 1.));
    }

    double r, g, b;
};

class Picture {
public:
    explicit Picture(size_t width, size_t height) : width(width), height(height) {
        colorMap.assign(width * height, Color());
    }

    explicit Picture() : width(0), height(0) {
    }

    Picture &operator=(Picture &&other) {
        this->colorMap = std::move(other.getColorMap());
        this->height = other.getHeight();
        this->width = other.getWidth();
        return *this;
    }

    Picture(Picture &&other) {
        *this = std::move(other);
    }

    const Color &getAt(size_t col, size_t row) const {
        assert(col >= 0 && row >= 0 && col < width && row < height);
        return colorMap[width * row + col];
    }

    void setAt(size_t col, size_t row, const Color &pixel) {
        assert(col >= 0 && row >= 0 && col < width && row < height);
        colorMap[width * row + col] = pixel;
    }

    size_t getHeight() const {
        return height;
    }

    size_t getWidth() const {
        return width;
    }

    const std::vector<Color> &getColorMap() const {
        return colorMap;
    }

    Picture mutiSempleAntiAliasing(size_t sampleNumber) {
        Picture newPicture(this->width / sampleNumber, this->height / sampleNumber); //todo: copy looks bad and slow
        std::cout << "Smoothing " << this->width <<'x' << this->height << "with MSAAx" << sampleNumber << "...\n";
        double pixelsPerSample = sampleNumber * sampleNumber;
        for (size_t col = 0; col < this->width / sampleNumber; ++col) {
            for (size_t row = 0; row < this->height / sampleNumber; ++row) {
                double r=0, g=0, b=0;
                for (size_t pixelCol = 0; pixelCol < sampleNumber; ++pixelCol) {
                    for (size_t pixelRow = 0; pixelRow < sampleNumber; ++pixelRow) {
                        auto subpixelColor = this->getAt(col * sampleNumber + pixelCol, row * sampleNumber + pixelRow);
                        r += subpixelColor.r;
                        g += subpixelColor.g;
                        b += subpixelColor.b;
                    }
                }
                Color newColor(r / pixelsPerSample, g / pixelsPerSample, b / pixelsPerSample);
                newPicture.setAt(col, row, newColor);
            }
        }
        std::cout << "Smoothing finished\n";
        return newPicture;
    }

private:
    size_t width, height;
    std::vector<Color> colorMap;
};

static const Color CL_WHITE = Color(1, 1, 1);
static const Color CL_BLACK = Color(0, 0, 0);
static const Color CL_RED = Color(1, 0, 0);
static const Color CL_GREEN = Color(0, 1, 0);
static const Color CL_BLUE = Color(0, 0, 1);

#endif //RAYTRACER_PICTURE_H
