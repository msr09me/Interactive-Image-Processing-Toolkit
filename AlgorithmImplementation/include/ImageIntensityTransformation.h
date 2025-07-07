#ifndef IMAGE_INTENSITY_TRANSFORMATION_H
#define IMAGE_INTENSITY_TRANSFORMATION_H

#include "ImageIO.h"

namespace iipt {

class ImageIntensityTransformation {
public:
    static void applyNegative(Image& img);
    static void applyLog(Image& img, float c);
    static void applyGamma(Image& img, float gamma, float c);
};

} // namespace iipt

#endif // IMAGE_INTENSITY_TRANSFORMATION_H
