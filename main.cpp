#include <iostream>
#include "Painter/CairoPainter.h"
#include <thread>
//
//void window(CairoPainter& cairoPainter) {
//    cairoPainter.show();
//}

int main( int argc, char *argv[])
{
    CairoPainter cairoPainter(800, 600, "Ray Tracer");
    Picture random(800, 600);
    for (size_t col = 0; col < random.getWidth(); ++col) {
        for (size_t row = 0; row < random.getHeight(); ++row) {
            if (col % 2 == 0) {
                random.setAt(col, row, Color(0, 0, 255));
            } else{
                random.setAt(col, row, Color(0, 0, 0));
            }
        }
    }

    cairoPainter.showPicture(random);
    return 0;
}