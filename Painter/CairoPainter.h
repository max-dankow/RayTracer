#ifndef RAYTRACER_CAIROPAINTER_H
#define RAYTRACER_CAIROPAINTER_H

#include <cairo.h>
#include <gtk/gtk.h>
#include <string>
#include "Painter.h"

class CairoPainter : public Painter {

public:
    CairoPainter(size_t screenWidth, size_t screenHeight, const std::string &windowTitle);

    virtual void showPicture(const Picture &picture);

private:
    std::string windowTitle;
    GtkWidget *window;
    GtkWidget *darea;
    static void onDrawEvent(GtkWidget *widget, cairo_t *cr, gpointer user_data);
    static void putPoint(size_t column, size_t row, const Color &color, cairo_t *cr);
};

#endif //RAYTRACER_CAIROPAINTER_H
