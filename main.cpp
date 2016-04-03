#include <iostream>
#include "Painter/CairoPainter.h"

int main( int argc, char *argv[])
{
    CairoPainter cairoPainter(800, 600, "Hello");
    cairoPainter.show();
    return 0;
}