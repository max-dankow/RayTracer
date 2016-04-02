#ifndef RAYTRACER_CAIROPAINTER_H
#define RAYTRACER_CAIROPAINTER_H

#include "Painter.h"
#include <cairo/cairo.h>

class CairoPainter : public Painter {

public:
    CairoPainter() { }

    virtual void putPoint(const ScreenPoint &point) override {

    }
};

#endif //RAYTRACER_CAIROPAINTER_H
