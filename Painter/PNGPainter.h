#ifndef RAYTRACER_PNGPAINTER_H
#define RAYTRACER_PNGPAINTER_H

#include "../png++/png.hpp"
#include "Painter.h"

class PNGPainter : public Painter {
public:
    PNGPainter(size_t screenWidth, size_t screenHeight, std::string filename) :
            Painter(screenWidth, screenHeight), fileName(filename) { }

    virtual void showPicture(const Picture &picture) {
        png::image<png::rgb_pixel> image(screenWidth, screenHeight);
        size_t width = std::min(picture.getWidth(), image.get_width());
        size_t height = std::min(picture.getHeight(), image.get_height());
        for (size_t col = 0; col < width; col++) {
            for (size_t row = 0; row < height; row++) {
                png::rgb_pixel pixel = image.get_pixel(col, row);
                Color color = picture.getAt(col, row);
                pixel.red = (png::byte) (color.r * 255);
                pixel.green = (png::byte) (color.g * 255);
                pixel.blue = (png::byte) (color.b * 255);
                image.set_pixel(col, row, pixel);
            }
        }
        image.write(fileName);
    }

    void setFileName(const std::string &fileName) {
        this->fileName = fileName;
    }

private:
    std::string fileName;
};


#endif //RAYTRACER_PNGPAINTER_H
