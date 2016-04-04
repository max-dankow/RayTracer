#ifndef RAYTRACER_CAIROPAINTER_H
#define RAYTRACER_CAIROPAINTER_H

#include "Painter.h"
#include <cairo.h>
#include <gtk/gtk.h>

class CairoPainter : public Painter {

public:
    CairoPainter(unsigned int screenWidth, unsigned int screenHeight, const std::string &windowTitle);

    virtual void showPicture(const Picture &picture);

private:
    std::string windowTitle;
    GtkWidget *window;
    GtkWidget *darea;
    static void onDrawEvent(GtkWidget *widget, cairo_t *cr, gpointer user_data);
    static void putPoint(const ImagePoint &point, cairo_t *cr);
};

#endif //RAYTRACER_CAIROPAINTER_H
