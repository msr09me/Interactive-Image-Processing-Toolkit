#include "ImageQtAdapter.h"
#include <stdexcept>

iipt::Image ImageQtAdapter::fromQImage(const QImage& qimage) {
    iipt::Image img;

    img.width = qimage.width();
    img.height = qimage.height();
    img.channels = qimage.isGrayscale() ? 1 : 3;
    img.data.resize(img.width * img.height * img.channels);

    for (int y = 0; y < qimage.height(); ++y) {
        for (int x = 0; x < qimage.width(); ++x) {
            QRgb pixel = qimage.pixel(x, y);
            int index = y * img.width + x;

            if (img.channels == 1) {
                img.data[index] = qGray(pixel);
            } else {
                index *= 3;
                img.data[index]     = qRed(pixel);
                img.data[index + 1] = qGreen(pixel);
                img.data[index + 2] = qBlue(pixel);
            }
        }
    }

    return img;
}

QImage ImageQtAdapter::toQImage(const iipt::Image& image) {
    if (image.channels == 1) {
        QImage qimage(image.width, image.height, QImage::Format_Grayscale8);
        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                int index = y * image.width + x;
                uchar val = image.data[index];
                qimage.setPixel(x, y, qRgb(val, val, val));
            }
        }
        return qimage;
    } else {
        throw std::runtime_error("Only grayscale images are supported for now.");
    }
}
