#include "ImageIO.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>

namespace iipt {

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t bfType;       // 'BM'
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;

    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

Image::Image() : width(0), height(0), channels(3) {}

bool Image::loadBMP(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open BMP file: " << filename << "\n";
        return false;
    }

    BMPHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    if (header.bfType != 0x4D42 || header.biCompression != 0) {
        std::cerr << "Unsupported BMP format\n";
        return false;
    }

    width = header.biWidth;
    height = std::abs(header.biHeight);
    bool isTopDown = header.biHeight < 0;

    const uint16_t bitDepth = header.biBitCount;

    if (bitDepth == 24) {
        channels = 3;
        int rowSize = ((width * 3 + 3) / 4) * 4;
        data.resize(width * height * channels);
        std::vector<unsigned char> row(rowSize);

        for (int y = 0; y < height; ++y) {
            file.read(reinterpret_cast<char*>(row.data()), rowSize);
            int rowIdx = isTopDown ? y : (height - 1 - y);
            for (int x = 0; x < width; ++x) {
                int src = x * 3;
                int dst = (rowIdx * width + x) * channels;
                data[dst + 0] = row[src + 2]; // R
                data[dst + 1] = row[src + 1]; // G
                data[dst + 2] = row[src + 0]; // B
            }
        }
    }
    else if (bitDepth == 8) {
        channels = 1;

        // Read 256-color palette (1024 bytes)
        std::vector<uint8_t> colorTable(1024);
        file.read(reinterpret_cast<char*>(colorTable.data()), 1024);

        int rowSize = ((width + 3) / 4) * 4; // 1 byte per pixel
        data.resize(width * height);
        std::vector<uint8_t> row(rowSize);

        for (int y = 0; y < height; ++y) {
            file.read(reinterpret_cast<char*>(row.data()), rowSize);
            int rowIdx = isTopDown ? y : (height - 1 - y);
            for (int x = 0; x < width; ++x) {
                uint8_t index = row[x];
                // You can extract RGB from palette here if needed
                // For now, we assume grayscale palette (R=G=B)
                data[rowIdx * width + x] = colorTable[index * 4]; // Just take the blue channel (same for R/G/B in grayscale)
            }
        }
    }
    else {
        std::cerr << "Unsupported bit depth: " << bitDepth << "\n";
        return false;
    }

    return true;
}


bool Image::saveBMP(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to save BMP file: " << filename << "\n";
        return false;
    }

    int rowSize = (channels == 3)
                    ? ((width * 3 + 3) / 4) * 4
                    : ((width + 3) / 4) * 4;

    int imageSize = rowSize * height;
    int colorTableSize = (channels == 1) ? 1024 : 0;

    BMPHeader header{};
    header.bfType = 0x4D42;
    header.bfSize = sizeof(BMPHeader) + colorTableSize + imageSize;
    header.bfOffBits = sizeof(BMPHeader) + colorTableSize;
    header.biSize = 40;
    header.biWidth = width;
    header.biHeight = height;
    header.biPlanes = 1;
    header.biBitCount = (channels == 1) ? 8 : 24;
    header.biCompression = 0;
    header.biSizeImage = imageSize;
    header.biXPelsPerMeter = 2835;
    header.biYPelsPerMeter = 2835;

    // Write BMP header
    file.write(reinterpret_cast<const char*>(&header), sizeof(BMPHeader));

    // Write grayscale color table if 8-bit
    if (channels == 1) {
        for (int i = 0; i < 256; ++i) {
            uint8_t gray[4] = { static_cast<uint8_t>(i), static_cast<uint8_t>(i), static_cast<uint8_t>(i), 0 };
            file.write(reinterpret_cast<char*>(gray), 4);
        }
    }

    // Write pixel data (bottom-up)
    std::vector<uint8_t> row(rowSize);
    for (int y = height - 1; y >= 0; --y) {
        if (channels == 1) {
            for (int x = 0; x < width; ++x) {
                row[x] = data[y * width + x];
            }
        } else { // RGB
            for (int x = 0; x < width; ++x) {
                int src = (y * width + x) * 3;
                row[x * 3 + 0] = data[src + 2]; // B
                row[x * 3 + 1] = data[src + 1]; // G
                row[x * 3 + 2] = data[src + 0]; // R
            }
        }
        file.write(reinterpret_cast<char*>(row.data()), rowSize);
    }

    file.close();
    return true;
}


} // namespace iipt
