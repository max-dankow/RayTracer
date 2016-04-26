#include "Picture.h"

Picture Picture::mutiSempleAntiAliasing(size_t sampleNumber) {
    Picture newPicture(this->width / sampleNumber, this->height / sampleNumber); //todo: copy looks bad and slow
    std::cout << "Smoothing " << this->width <<'x' << this->height << "with MSAAx" << sampleNumber << "...\n";
    double pixelsPerSample = sampleNumber * sampleNumber;
    for (size_t col = 0; col < this->width / sampleNumber; ++col) {
        for (size_t row = 0; row < this->height / sampleNumber; ++row) {
            double r=0, g=0, b=0;
            for (size_t pixelCol = 0; pixelCol < sampleNumber; ++pixelCol) {
                for (size_t pixelRow = 0; pixelRow < sampleNumber; ++pixelRow) {
                    auto subpixelColor = this->getAt(col * sampleNumber + pixelCol, row * sampleNumber + pixelRow);
                    r += subpixelColor.r;
                    g += subpixelColor.g;
                    b += subpixelColor.b;
                }
            }
            Color newColor(r / pixelsPerSample, g / pixelsPerSample, b / pixelsPerSample);
            newPicture.setAt(col, row, newColor);
        }
    }
    std::cout << "Smoothing finished\n";
    return newPicture;
}
