#ifndef RAYTRACER_PAINTER_H
#define RAYTRACER_PAINTER_H

#include "../Picture.h"

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
