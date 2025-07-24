#include "ImageMorphology.h"
#include "ImageUtils.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>

namespace iipt {

namespace {
// Helper function to check SE match for erosion
bool fits(const std::vector<unsigned char>& data, int width, int height,
          int x, int y, const std::vector<std::vector<int>>& se) {
    int kH = se.size();
    int kW = se[0].size();
    int kCX = kW / 2;
    int kCY = kH / 2;

    for (int dy = 0; dy < kH; ++dy) {
        for (int dx = 0; dx < kW; ++dx) {
            if (se[dy][dx] == 1) {
                int ix = x + dx - kCX;
                int iy = y + dy - kCY;
                if (ix < 0 || iy < 0 || ix >= width || iy >= height ||
                    data[iy * width + ix] == 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// Helper function to check SE match for dilation
bool hits(const std::vector<unsigned char>& data, int width, int height,
          int x, int y, const std::vector<std::vector<int>>& se) {
    int kH = se.size();
    int kW = se[0].size();
    int kCX = kW / 2;
    int kCY = kH / 2;

    for (int dy = 0; dy < kH; ++dy) {
        for (int dx = 0; dx < kW; ++dx) {
            if (se[dy][dx] == 1) {
                int ix = x + dx - kCX;
                int iy = y + dy - kCY;
                if (ix >= 0 && iy >= 0 && ix < width && iy < height &&
                    data[iy * width + ix] > 0) {
                    return true;
                }
            }
        }
    }
    return false;
}
} // anonymous namespace

void ImageMorphology::erosion(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding) {
    if (img.channels != 1) {
        std::cerr << "Erosion only supports grayscale/binary images.\n";
        return;
    }

    int kH = se.size();
    int kW = se[0].size();
    int padY = kH / 2;
    int padX = kW / 2;

    std::vector<unsigned char> padded = ImageUtils::padImage(img.data, img.width, img.height, 1, padX, padY, padding);
    int paddedWidth = img.width + 2 * padX;
    int paddedHeight = img.height + 2 * padY;

    std::vector<unsigned char> output(img.width * img.height, 0);

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            if (fits(padded, paddedWidth, paddedHeight, x + padX, y + padY, se))
                output[y * img.width + x] = 255;
        }
    }

    img.data = std::move(output);
}

void ImageMorphology::dilation(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding) {
    if (img.channels != 1) {
        std::cerr << "Dilation only supports grayscale/binary images.\n";
        return;
    }

    int kH = se.size();
    int kW = se[0].size();
    int padY = kH / 2;
    int padX = kW / 2;

    std::vector<unsigned char> padded = ImageUtils::padImage(img.data, img.width, img.height, 1, padX, padY, padding);
    int paddedWidth = img.width + 2 * padX;
    int paddedHeight = img.height + 2 * padY;

    std::vector<unsigned char> output(img.width * img.height, 0);

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            if (hits(padded, paddedWidth, paddedHeight, x + padX, y + padY, se))
                output[y * img.width + x] = 255;
        }
    }

    img.data = std::move(output);
}

void ImageMorphology::opening(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding) {
    std::vector<unsigned char> original = img.data;

    erosion(img, se, padding);
    dilation(img, se, padding);

    if (img.data.empty())
        img.data = std::move(original);
}

void ImageMorphology::closing(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding) {
    std::vector<unsigned char> original = img.data;

    dilation(img, se, padding);
    erosion(img, se, padding);

    if (img.data.empty())
        img.data = std::move(original);
}

void ImageMorphology::boundaryExtract(Image& img, const std::vector<std::vector<int>>& se, ImageUtils::PaddingType padding) {
    if (img.channels != 1) {
        std::cerr << "Boundary extraction only supports grayscale/binary images.\n";
        return;
    }

    std::vector<unsigned char> original = img.data;
    Image temp = img;
    temp.data = original;

    erosion(temp, se, padding);

    for (size_t i = 0; i < img.data.size(); ++i) {
        img.data[i] = std::clamp<int>(original[i] - temp.data[i], 0, 255);
    }
}

} // namespace iipt