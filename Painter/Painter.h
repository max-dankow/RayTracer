#ifndef RAYTRACER_PAINTER_H
#define RAYTRACER_PAINTER_H

#include <vector>
#include <assert.h>

struct Color {
    Color(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) : r(r), g(g), b(b) { }

    unsigned char r, g, b;
};

class Picture {
public:
    explicit Picture(size_t width, size_t height) : width(width), height(height) {
        colorMap.assign(width * height, Color());
    }

    const Color &getAt(size_t col, size_t row) const {
        assert(col >= 0 && row >= 0 && col < width && row < height);
        return colorMap[width * row + col];
    }

    void setAt(size_t col, size_t row, const Color &pixel) {
        assert(col >= 0 && row >= 0 && col < width && row < height);
        colorMap[width * row + col] = pixel;
    }

private:
    size_t width, height;
    std::vector<Color> colorMap;
};

struct ScreenPoint {

    ScreenPoint(int x, int y, const Color &color = Color()) : x(x), y(y), color(color) { }

    int x, y;
    Color color;
};

class Painter {
public:
    Painter(unsigned screenWidth, unsigned screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight) { }

    Painter(const Painter &other) = delete;

protected:
    unsigned screenWidth, screenHeight;
};

#endif //RAYTRACER_PAINTER_H
