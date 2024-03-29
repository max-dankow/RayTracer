#include <string>
#include <iostream>
#include "CairoPainter.h"

CairoPainter::CairoPainter(size_t screenWidth,
                           size_t screenHeight,
                           const std::string &windowTitle) :
        Painter(screenWidth, screenHeight),
        windowTitle(windowTitle) { }

void CairoPainter::onDrawEvent(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    Picture *picture = (Picture *) user_data;
    for (size_t col = 0; col < picture->getWidth(); ++col) {
        for (size_t row = 0; row < picture->getHeight(); ++row) {
            putPoint(col, row, picture->getAt(col, row), cr);
        }
    }
}

void CairoPainter::showPicture(const Picture &picture) {
    gtk_init(NULL, NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);

    g_signal_connect(G_OBJECT(darea), "draw",
                     G_CALLBACK(onDrawEvent),
                     const_cast<Picture*> (&picture));

    g_signal_connect(window, "destroy",
                     G_CALLBACK(gtk_main_quit),
                     NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), (gint) screenWidth, (gint) screenHeight);
    gtk_window_set_title(GTK_WINDOW(window), windowTitle.c_str());
    gtk_widget_show_all(window);
    gtk_main();
}

// Рисует пиксель с указанными координатами указанным цветом.
void CairoPainter::putPoint(size_t column, size_t row, const Color &color, cairo_t *cr) {
    const size_t PIXEL_SIZE = 1;
    cairo_set_source_rgb(cr, color.r, color.g, color.b);
    cairo_rectangle(cr, column * PIXEL_SIZE, row * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE);
    cairo_fill(cr);
    cairo_stroke(cr);
}
