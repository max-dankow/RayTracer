#ifndef RAYTRACER_PAINTER_H
#define RAYTRACER_PAINTER_H

#include <vector>
#include <assert.h>

struct Color {
    Color(double r = 0, double g = 0, double b = 0) : r(r), g(g), b(b) {
        assert(r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1);
    }

    double r, g, b;
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

    size_t getHeight() const {
        return height;
    }

    size_t getWidth() const {
        return width;
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

// Интерфейс Painter предоставляет отрисовку Picture на холсте заданного рамера.
class Painter {
public:
    Painter(unsigned screenWidth, unsigned screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight) { }

    Painter(const Painter &) = delete;

    virtual void showPicture(const Picture &)=0;

protected:
    unsigned screenWidth, screenHeight;
};

#endif //RAYTRACER_PAINTER_H
