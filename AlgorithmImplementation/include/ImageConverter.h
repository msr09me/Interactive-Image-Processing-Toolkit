#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include "ImageIO.h"

namespace iipt {

class RGBToGrayscaleConverter {
    public:
        static void convert(Image& img); 
};

class GrayscaleToBinaryConverter {
    public:
        static void fixedThreshold(Image& img, int threshold);
        static void otsuThreshold(Image& img);
        static void adaptiveMeanThreshold(Image& img, int blockSize, int C);
        static void adaptiveGaussianThreshold(Image& img, int blockSize, int C);
};

} // namespace iipt

#endif // IMAGECONVERTER_H
