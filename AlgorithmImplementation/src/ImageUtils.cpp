#include "ImageUtils.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace iipt {

    ImageUtils::PaddingType ImageUtils::askPaddingType() {
    std::cout << "Choose padding type:\n"
              << "1. None\n"
              << "2. Zero Padding\n"
              << "3. Replicate\n"
              << "4. Mirror\n"
              << "Type the number: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1: return ImageUtils::PaddingType::None;
        case 2: return ImageUtils::PaddingType::Zero;
        case 3: return ImageUtils::PaddingType::Replicate;
        case 4: return ImageUtils::PaddingType::Mirror;
        default: throw std::runtime_error("Invalid padding type.");
    }
}


    std::vector<unsigned char> ImageUtils::padImage(
        const std::vector<unsigned char>& data,
        int width, int height,
        int channels,
        int padX, int padY,
        PaddingType type
    ) {
        int newWidth = width + 2 * padX;
        int newHeight = height + 2 * padY;
        std::vector<unsigned char> padded(newWidth * newHeight * channels, 0);

        auto getPixel = [&](int x, int y, int c) -> unsigned char {
            switch (type) {
            case PaddingType::Zero:
                if (x < 0 || y < 0 || x >= width || y >= height) return 0;
                return data[(y * width + x) * channels + c];
            case PaddingType::Replicate:
                x = std::clamp(x, 0, width - 1);
                y = std::clamp(y, 0, height - 1);
                return data[(y * width + x) * channels + c];
            case PaddingType::Mirror:
                x = (x < 0) ? -x : (x >= width ? 2 * width - x - 2 : x);
                y = (y < 0) ? -y : (y >= height ? 2 * height - y - 2 : y);
                return data[(y * width + x) * channels + c];
            case PaddingType::None:
            default:
                std::cerr << "Warning: PaddingType::None should not be used here. Returning zeros.\n";
                return 0;
            }
        };

        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                for (int c = 0; c < channels; ++c) {
                    padded[(y * newWidth + x) * channels + c] = getPixel(x - padX, y - padY, c);
                }
            }
        }

        return padded;
    }



std::vector<std::vector<int>> ImageUtils::createStructuringElement(const std::string& shape, int size) {
    if (size % 2 == 0 || size <= 1) {
        std::cerr << "Invalid SE size. Must be odd and > 1. Defaulting to size = 3.\n";
        size = 3;
        }

        std::vector<std::vector<int>> se(size, std::vector<int>(size, 0));
        int center = size / 2;

        if (shape == "square") {
            for (int y = 0; y < size; ++y)
                for (int x = 0; x < size; ++x)
                    se[y][x] = 1;
        } 
        else if (shape == "cross") {
            for (int i = 0; i < size; ++i) {
                se[center][i] = 1;  // horizontal line
                se[i][center] = 1;  // vertical line
            }
        } 
        else if (shape == "circle") {
            float radius = static_cast<float>(size) / 2.0f;
            for (int y = 0; y < size; ++y) {
                for (int x = 0; x < size; ++x) {
                    float dx = x - center;
                    float dy = y - center;
                    if ((dx * dx + dy * dy) <= radius * radius) {
                        se[y][x] = 1;
                    }
                }
            }
        } 
        else if (shape == "line_horizontal") {
            for (int x = 0; x < size; ++x)
                se[center][x] = 1;
        } 
        else if (shape == "line_vertical") {
            for (int y = 0; y < size; ++y)
                se[y][center] = 1;
        } 
        else {
            std::cerr << "Unknown SE shape '" << shape << "'. Defaulting to square.\n";
            for (int y = 0; y < size; ++y)
                for (int x = 0; x < size; ++x)
                    se[y][x] = 1;
        }

        return se;
    }

}
