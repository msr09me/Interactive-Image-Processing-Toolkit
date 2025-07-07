#ifndef IMAGE_QT_ADAPTER_H
#define IMAGE_QT_ADAPTER_H

#include <QImage>
#include "ImageIO.h"  // From AlgorithmImplementation/include

class ImageQtAdapter {
public:
    // Convert QImage to iipt::Image
    static iipt::Image fromQImage(const QImage& qimage);

    // Convert iipt::Image to QImage
    static QImage toQImage(const iipt::Image& image);
};

#endif // IMAGE_QT_ADAPTER_H
