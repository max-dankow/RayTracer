#ifndef RAYTRACER_PAINTER_H
#define RAYTRACER_PAINTER_H

struct Color {
    Color(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) : r(r), g(g), b(b) { }

    unsigned char r, g, b;
};

struct ScreenPoint {
    int x, y;
    Color color;
};

class Painter {
public:
    Painter() { }

    virtual void putPoint(const ScreenPoint &point) = 0;
//    void createWindow
};

#endif //RAYTRACER_PAINTER_H
