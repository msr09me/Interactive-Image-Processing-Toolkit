#include "ImageSpatialTransformation.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace iipt {

// -------------------- For Users ------------------------------------------------------

void SpatialTransformation::applyBoxFilter(Image& img, int kernelSize, PaddingType padding) {
    img.data = boxFilterCore(img.data, img.width, img.height, img.channels, kernelSize, padding);
}

void SpatialTransformation::applyGaussianFilter(Image& img, int kernelSize, float sigma, PaddingType padding) {
    img.data = gaussianFilterCore(img.data, img.width, img.height, img.channels, kernelSize, padding, sigma);
}

void SpatialTransformation::applyMedianFilter(Image& img, int kernelSize, PaddingType padding) {
    img.data = medianFilterCore(img.data, img.width, img.height, img.channels, kernelSize, padding);
}

void SpatialTransformation::applyLaplacianBasic(Image& img, bool inverted, PaddingType padding) {
    img.data = laplacianBasicCore(img.data, img.width, img.height, img.channels, inverted, padding);
}

void SpatialTransformation::applyLaplacianFull(Image& img, bool inverted, PaddingType padding) {
    img.data = laplacianFullCore(img.data, img.width, img.height, img.channels, inverted, padding);
}

void SpatialTransformation::applySobel(Image& img, PaddingType padding) {
    img.data = sobelCore(img.data, img.width, img.height, img.channels, padding);
}

void SpatialTransformation::applySharpening(Image& img, const std::string& method, PaddingType padding) {
    img.data = sharpeningCore(img.data, img.width, img.height, img.channels, method, padding);
}

void SpatialTransformation::applyUnsharpMasking(Image& img, const std::string& kernelType, int kernelSize, float sigma, PaddingType padding) {
    img.data = unsharpMaskingCore(img.data, img.width, img.height, img.channels, kernelType, kernelSize, sigma, padding);
}

void SpatialTransformation::applyHighboostFiltering(Image& img, const std::string& kernelType, int kernelSize, float K, float sigma, PaddingType padding) {
    img.data = highboostFilteringCore(img.data, img.width, img.height, img.channels, kernelType, kernelSize, K, sigma, padding);
}

// -------------------- Algorithm Implementations ----------------------

std::vector<unsigned char> SpatialTransformation::boxFilterCore(const std::vector<unsigned char>& input, 
                                                                int width, int height, int channels, 
                                                                int kernelSize, PaddingType padding) {
    std::vector<std::vector<float>> kernel(kernelSize, std::vector<float>(kernelSize, 1.0f / (kernelSize * kernelSize)));
    return convolve(input, width, height, channels, kernel, padding);
}

std::vector<unsigned char> SpatialTransformation::gaussianFilterCore(const std::vector<unsigned char>& input, 
                                                                    int width, int height, int channels, int kernelSize, PaddingType padding, float sigma) {
    auto kernel = generateGaussianKernel(kernelSize, sigma);
    return convolve(input, width, height, channels, kernel, padding);
}

std::vector<unsigned char> SpatialTransformation::medianFilterCore(const std::vector<unsigned char>& input,
                                                                    int width, int height, int channels,
                                                                    int kernelSize, PaddingType padding)
{
    int k = kernelSize / 2;
    std::vector<unsigned char> output(width * height * channels, 0);

    int startY = (padding == PaddingType::None) ? k : 0;
    int endY   = (padding == PaddingType::None) ? height - k : height;
    int startX = (padding == PaddingType::None) ? k : 0;
    int endX   = (padding == PaddingType::None) ? width - k : width;

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            for (int c = 0; c < channels; ++c) {
                std::vector<unsigned char> neighborhood;

                for (int dy = -k; dy <= k; ++dy) {
                    for (int dx = -k; dx <= k; ++dx) {
                        int px = x + dx;
                        int py = y + dy;

                        if (px < 0 || px >= width || py < 0 || py >= height) {
                            if (padding == PaddingType::None) continue;
                            else if (padding == PaddingType::Zero) {
                                neighborhood.push_back(0);
                                continue;
                            }
                            else if (padding == PaddingType::Replicate) {
                                px = std::clamp(px, 0, width - 1);
                                py = std::clamp(py, 0, height - 1);
                            }
                            else if (padding == PaddingType::Mirror) {
                                if (px < 0) px = -px;
                                if (px >= width) px = 2 * width - px - 2;
                                if (py < 0) py = -py;
                                if (py >= height) py = 2 * height - py - 2;
                            }
                        }

                        int idx = (py * width + px) * channels + c;
                        neighborhood.push_back(input[idx]);
                    }
                }

                std::sort(neighborhood.begin(), neighborhood.end());
                int outIdx = (y * width + x) * channels + c;
                output[outIdx] = neighborhood.empty() ? 0 : neighborhood[neighborhood.size() / 2];
            }
        }
    }

    return output;
}


std::vector<unsigned char> SpatialTransformation::laplacianBasicCore(const std::vector<unsigned char>& input, 
                                                                    int width, int height, int channels, bool inverted, PaddingType padding) {
    std::vector<std::vector<float>> kernel = {
        { 0, -1, 0 },
        {-1,  4, -1},
        { 0, -1, 0 }
    };

    if (inverted)
        for (auto& row : kernel) for (float& v : row) v *= -1;

    return convolve(input, width, height, channels, kernel, padding);
}

std::vector<unsigned char> SpatialTransformation::laplacianFullCore(const std::vector<unsigned char>& input, 
        int width, int height, int channels, bool inverted, PaddingType padding) {
    std::vector<std::vector<float>> kernel = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    if (inverted)
        for (auto& row : kernel) for (float& v : row) v *= -1;

    return convolve(input, width, height, channels, kernel, padding);
}

std::vector<unsigned char> SpatialTransformation::sobelCore(const std::vector<unsigned char>& input, 
                                                            int width, int height, int channels, PaddingType padding) {
    std::vector<unsigned char> output(width * height * channels, 0);

    std::vector<std::vector<int>> Gx = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    std::vector<std::vector<int>> Gy = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            for (int c = 0; c < channels; ++c) {
                float gx = 0, gy = 0;
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int px = x + kx;
                        int py = y + ky;
                        int idx = (py * width + px) * channels + c;
                        gx += input[idx] * Gx[ky + 1][kx + 1];
                        gy += input[idx] * Gy[ky + 1][kx + 1];
                    }
                }
                float mag = std::sqrt(gx * gx + gy * gy);
                output[(y * width + x) * channels + c] = static_cast<unsigned char>(std::clamp(mag, 0.0f, 255.0f));
            }
        }
    }

    return output;
}

std::vector<unsigned char> SpatialTransformation::sharpeningCore(const std::vector<unsigned char>& input, 
                                                                int width, int height, int channels, const std::string& method, PaddingType padding) {
    std::vector<unsigned char> edge;

    if      (method == "Basic Laplacian")           edge = laplacianBasicCore(input, width, height, channels, true, padding);
    else if (method == "Full Laplacian")            edge = laplacianFullCore(input, width, height, channels, true, padding);
    else if (method == "Basic Inverted Laplacian")  edge = laplacianBasicCore(input, width, height, channels, false, padding);
    else if (method == "Full Inverted Laplacian")   edge = laplacianFullCore(input, width, height, channels, false, padding);
    else if (method == "Sobel")                     edge = sobelCore(input, width, height, channels, padding);
    else throw std::runtime_error("Unknown sharpening method");

    std::vector<unsigned char> output(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        int val = static_cast<int>(input[i]) + static_cast<int>(edge[i]);
        output[i] = static_cast<unsigned char>(std::clamp(val, 0, 255));
    }

    return output;
}

std::vector<unsigned char> SpatialTransformation::unsharpMaskingCore(const std::vector<unsigned char>& input, 
                                                                    int width, int height, int channels, 
                                                                    const std::string& kernelType, int kernelSize, 
                                                                    float sigma, PaddingType padding) {
    std::vector<unsigned char> blurred;

    if      (kernelType == "box")     blurred = boxFilterCore(input, width, height, channels, kernelSize, padding);
    else if (kernelType == "gaussian") blurred = gaussianFilterCore(input, width, height, channels, kernelSize, padding, sigma);
    else if (kernelType == "median")  blurred = medianFilterCore(input, width, height, channels, kernelSize, padding);
    else throw std::runtime_error("Unknown kernel type for unsharp masking");

    std::vector<unsigned char> output(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        int mask = static_cast<int>(input[i]) - static_cast<int>(blurred[i]);
        int val = static_cast<int>(input[i]) + mask;
        output[i] = static_cast<unsigned char>(std::clamp(val, 0, 255));
    }

    return output;
}

std::vector<unsigned char> SpatialTransformation::highboostFilteringCore(const std::vector<unsigned char>& input, 
                                                                        int width, int height, int channels, 
                                                                        const std::string& kernelType, int kernelSize, 
                                                                        float K, float sigma, PaddingType padding) {
    std::vector<unsigned char> blurred;

    if      (kernelType == "box")     blurred = boxFilterCore(input, width, height, channels, kernelSize, padding);
    else if (kernelType == "gaussian") blurred = gaussianFilterCore(input, width, height, channels, kernelSize, padding, sigma);
    else if (kernelType == "median")  blurred = medianFilterCore(input, width, height, channels, kernelSize, padding);
    else throw std::runtime_error("Unknown kernel type for highboost filtering");

    std::vector<unsigned char> output(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        int mask = static_cast<int>(input[i]) - static_cast<int>(blurred[i]);
        int val = static_cast<int>(input[i]) + static_cast<int>(K * mask);
        output[i] = static_cast<unsigned char>(std::clamp(val, 0, 255));
    }

    return output;
}

// -------------------- KERNEL & CONVOLUTION ----------------------

std::vector<std::vector<float>> SpatialTransformation::generateGaussianKernel(int size, float sigma) {
    int k = size / 2;
    std::vector<std::vector<float>> kernel(size, std::vector<float>(size));
    float sum = 0.0f;

    for (int y = -k; y <= k; ++y) {
        for (int x = -k; x <= k; ++x) {
            float value = std::exp(-(x * x + y * y) / (2 * sigma * sigma));
            kernel[y + k][x + k] = value;
            sum += value;
        }
    }

    for (auto& row : kernel)
        for (auto& val : row)
            val /= sum;

    return kernel;
}

std::vector<unsigned char> SpatialTransformation::convolve(const std::vector<unsigned char>& input,
                                                            int width, int height, int channels,
                                                            const std::vector<std::vector<float>>& kernel,
                                                            PaddingType padding)
{
    int k = kernel.size() / 2;
    std::vector<unsigned char> output(width * height * channels, 0);

    int startY = (padding == PaddingType::None) ? k : 0;
    int endY   = (padding == PaddingType::None) ? height - k : height;
    int startX = (padding == PaddingType::None) ? k : 0;
    int endX   = (padding == PaddingType::None) ? width - k : width;

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0f;

                for (int ky = -k; ky <= k; ++ky) {
                    for (int kx = -k; kx <= k; ++kx) {
                        int px = x + kx;
                        int py = y + ky;

                        // Handle padding
                        if (px < 0 || px >= width || py < 0 || py >= height) {
                            if (padding == PaddingType::None) continue;
                            else if (padding == PaddingType::Zero) {
                                continue; // zero padding
                            }
                            else if (padding == PaddingType::Replicate) {
                                px = std::clamp(px, 0, width - 1);
                                py = std::clamp(py, 0, height - 1);
                            }
                            else if (padding == PaddingType::Mirror) {
                                if (px < 0) px = -px;
                                if (px >= width) px = 2 * width - px - 2;
                                if (py < 0) py = -py;
                                if (py >= height) py = 2 * height - py - 2;
                            }
                        }

                        int idx = (py * width + px) * channels + c;
                        sum += input[idx] * kernel[ky + k][kx + k];
                    }
                }

                int outIdx = (y * width + x) * channels + c;
                output[outIdx] = static_cast<unsigned char>(std::clamp(sum, 0.0f, 255.0f));
            }
        }
    }

    return output;
}

SpatialTransformation::PaddingType SpatialTransformation::askPaddingType() {
    std::cout << "Choose padding type:\n"
              << "1. None\n"
              << "2. Zero Padding\n"
              << "3. Replicate\n"
              << "4. Mirror\n"
              << "Type the number: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1: return SpatialTransformation::PaddingType::None;
        case 2: return SpatialTransformation::PaddingType::Zero;
        case 3: return SpatialTransformation::PaddingType::Replicate;
        case 4: return SpatialTransformation::PaddingType::Mirror;
        default: throw std::runtime_error("Invalid padding type.");
    }
}



} // namespace iipt
