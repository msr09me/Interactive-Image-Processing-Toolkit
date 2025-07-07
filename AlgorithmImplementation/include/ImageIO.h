#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include <string>
#include <vector>

namespace iipt {

class Image {
public:
    int width;
    int height;
    int channels; // e.g., 3 for RGB
    std::vector<unsigned char> data; // Pixel data in row-major order (RGBRGB...)

    Image();

    bool loadBMP(const std::string& filename);
    bool saveBMP(const std::string& filename) const;

};

} // namespace iipt

#endif // IMAGE_IO_H
