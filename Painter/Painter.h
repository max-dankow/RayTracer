#ifndef RAYTRACER_PAINTER_H
#define RAYTRACER_PAINTER_H

#include <vector>
#include <assert.h>
#include "../Scene.h"

// Представляет пиксель на картинке.
struct ImagePoint {

    ImagePoint(int x, int y, const Color &color = Color()) : x(x), y(y), color(color) { }

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
