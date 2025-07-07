#include "ImageIntensityTransformation.h"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace iipt {

void ImageIntensityTransformation::applyNegative(Image& img) {
    for (auto& pixel : img.data) {
        pixel = 255 - pixel;
    }
}

void ImageIntensityTransformation::applyLog(Image& img, float c) {
    for (auto& pixel : img.data) {
        pixel = static_cast<unsigned char>(
            std::clamp(c * std::log(1.0f + static_cast<float>(pixel)), 0.0f, 255.0f)
        );
    }
}

void ImageIntensityTransformation::applyGamma(Image& img, float gamma, float c) {
    float invGamma = 1.0f / gamma;
    for (auto& pixel : img.data) {
        float normalized = static_cast<float>(pixel) / 255.0f;
        float transformed = c * std::pow(normalized, invGamma) * 255.0f;
        pixel = static_cast<unsigned char>(std::clamp(transformed, 0.0f, 255.0f));
    }
}

} // namespace iipt
