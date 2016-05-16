#ifndef RAYTRACER_PAINTER_H
#define RAYTRACER_PAINTER_H

#include "../Picture.h"

// Интерфейс Painter предоставляет отрисовку Picture на холсте заданного рамера.
class Painter {
public:
    Painter(size_t screenWidth, size_t screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight) { }

    Painter(const Painter &) = delete;

    virtual void showPicture(const Picture &)=0;

protected:
    size_t screenWidth, screenHeight;
};

#endif //RAYTRACER_PAINTER_H
