#include <string>
#include <iostream>
#include "CairoPainter.h"

CairoPainter::CairoPainter(unsigned int screenWidth,
                           unsigned int screenHeight,
                           const std::string &windowTitle) : Painter(screenWidth, screenHeight) {
    gtk_init(NULL, NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);

    g_signal_connect(G_OBJECT(darea), "draw",
                     G_CALLBACK(onDrawEvent), this);
    g_signal_connect(window, "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), screenWidth, screenHeight);
    gtk_window_set_title(GTK_WINDOW(window), windowTitle.c_str());
}

gboolean CairoPainter::onDrawEvent(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    CairoPainter *owner = (CairoPainter *) user_data;
    cairo_set_source_rgb(cr, 0, 0, 0);
    for (int x = 0; x < owner->screenWidth / 2; x+=1) {
        for (int y = 0; y < owner->screenHeight / 2; y+=10) {
            putPoint(ScreenPoint(x, y, Color(255,0,0)), cr);
        }
    }
    return FALSE;
}

void CairoPainter::show() {
    gtk_widget_show_all(window);
    gtk_main();
}

void CairoPainter::putPoint(const ScreenPoint &point, cairo_t *cr) {
    cairo_set_source_rgb(cr, point.color.r, point.color.g, point.color.b);
    cairo_rectangle(cr, point.x, point.y, 1.0, 1.0);
    cairo_fill (cr);
}