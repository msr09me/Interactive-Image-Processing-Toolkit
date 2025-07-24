#include "ImageConverter.h"
#include <vector>
#include <cmath>
#include <algorithm>

namespace iipt {

// ========== RGB to GRAYSCALE ==========
void RGBToGrayscaleConverter::convert(Image& img) {
    if (img.channels != 3) return;

    std::vector<unsigned char> gray(img.width * img.height);
    for (int i = 0; i < img.width * img.height; ++i) {
        int idx = i * 3;
        unsigned char r = img.data[idx];
        unsigned char g = img.data[idx + 1];
        unsigned char b = img.data[idx + 2];
        gray[i] = static_cast<unsigned char>(0.299 * r + 0.587 * g + 0.114 * b);
    }

    img.data = gray;
    img.channels = 1;
}

// ========== FIXED THRESHOLD ==========
void GrayscaleToBinaryConverter::fixedThreshold(Image& img, int threshold) {
    if (img.channels != 1) return;

    for (auto& px : img.data)
        px = (px >= threshold) ? 255 : 0;
}

// ========== OTSU ==========
void GrayscaleToBinaryConverter::otsuThreshold(Image& img) {
    if (img.channels != 1) return;

    // Compute histogram
    int hist[256] = {0};
    for (unsigned char val : img.data) hist[val]++;

    int total = img.data.size();
    float sum = 0;
    for (int t = 0; t < 256; ++t) sum += t * hist[t];

    float sumB = 0, wB = 0, wF = 0, maxVar = 0;
    int threshold = 0;

    for (int t = 0; t < 256; ++t) {
        wB += hist[t];
        if (wB == 0) continue;
        wF = total - wB;
        if (wF == 0) break;

        sumB += t * hist[t];
        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;

        float varBetween = wB * wF * (mB - mF) * (mB - mF);
        if (varBetween > maxVar) {
            maxVar = varBetween;
            threshold = t;
        }
    }

    // Apply threshold
    for (auto& px : img.data)
        px = (px >= threshold) ? 255 : 0;
}

// ========== ADAPTIVE MEAN ==========
void GrayscaleToBinaryConverter::adaptiveMeanThreshold(Image& img, int blockSize, int C) {
    if (img.channels != 1) return;

    std::vector<unsigned char> original = img.data;
    std::vector<unsigned char>& output = img.data;
    int width = img.width, height = img.height;
    output.resize(width * height);

    int half = blockSize / 2;

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            int sum = 0, count = 0;

            for (int dy = -half; dy <= half; ++dy)
                for (int dx = -half; dx <= half; ++dx) {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        sum += original[ny * width + nx];
                        count++;
                    }
                }

            int mean = count ? (sum / count) : 0;
            unsigned char threshold = static_cast<unsigned char>(mean - C);
            output[y * width + x] = (original[y * width + x] >= threshold) ? 255 : 0;
        }
}

// ========== ADAPTIVE GAUSSIAN ==========
void GrayscaleToBinaryConverter::adaptiveGaussianThreshold(Image& img, int blockSize, int C) {
    if (img.channels != 1) return;

    std::vector<unsigned char> original = img.data;
    std::vector<unsigned char>& output = img.data;
    int width = img.width, height = img.height;
    output.resize(width * height);

    int half = blockSize / 2;
    float sigma = blockSize / 6.0f;

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            float sum = 0, weightSum = 0;

            for (int dy = -half; dy <= half; ++dy)
                for (int dx = -half; dx <= half; ++dx) {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        float weight = std::exp(-(dx * dx + dy * dy) / (2 * sigma * sigma));
                        sum += original[ny * width + nx] * weight;
                        weightSum += weight;
                    }
                }

            int threshold = (weightSum != 0) ? (sum / weightSum - C) : 0;
            output[y * width + x] = (original[y * width + x] >= threshold) ? 255 : 0;
        }
}

} // namespace iipt
